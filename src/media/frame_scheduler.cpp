#include <synthesizer/media/frame_scheduler.hpp>

#include <limits>

namespace synthesizer::media {

FrameScheduler::FrameScheduler(TimeBase global_time_base)
    : global_time_base_(global_time_base) {}

// 简单的时间基转换：将 ts * src_tb 转到 global_time_base_ 下
// 注意：这是一个非常简化的实现，用整数运算，未做溢出保护。
// 在生产环境建议改用 FFmpeg 的 av_rescale_q。
int64_t FrameScheduler::toGlobalTimeUs(int64_t ts,
                                       const TimeBase &src_tb) const {
  // ts * (src.num/src.den) * (global.den/global.num)
  // 目标 global_time_base_ 默认是 1/1_000_000
  if (src_tb.den == 0 || global_time_base_.num == 0) {
    return 0;
  }

  // 为减少溢出风险，先做 64 位扩展
  long double value = static_cast<long double>(ts) *
                      static_cast<long double>(src_tb.num) /
                      static_cast<long double>(src_tb.den) *
                      static_cast<long double>(global_time_base_.den) /
                      static_cast<long double>(global_time_base_.num);

  if (value > static_cast<long double>(std::numeric_limits<int64_t>::max())) {
    return std::numeric_limits<int64_t>::max();
  }
  if (value < static_cast<long double>(std::numeric_limits<int64_t>::min())) {
    return std::numeric_limits<int64_t>::min();
  }
  return static_cast<int64_t>(value);
}

int64_t FrameScheduler::nowWallTimeUs() const {
  auto now = SteadyClock::now();
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(
      now.time_since_epoch());
  return us.count();
}

void FrameScheduler::pushFrame(const MediaFrame &frame) {
  auto it = tracks_.find(frame.track_id);
  if (it == tracks_.end()) {
    TrackState ts;
    ts.track_id = frame.track_id;
    ts.type = frame.type;
    ts.time_base = frame.time_base;
    tracks_.emplace(frame.track_id, std::move(ts));
    it = tracks_.find(frame.track_id);
  }

  it->second.queue.push_back(frame);
}

void FrameScheduler::play() {
  if (playback_.state == PlayState::Playing) {
    return;
  }

  playback_.state = PlayState::Playing;
  initClockIfNeeded();
}

void FrameScheduler::pause() { playback_.state = PlayState::Paused; }

void FrameScheduler::stop() { playback_.state = PlayState::Stopped; }

void FrameScheduler::setRate(double rate) {
  if (rate <= 0.0) {
    return;
  }
  playback_.rate = rate;
}

void FrameScheduler::reset() {
  playback_ = PlaybackState{};
  tracks_.clear();
  clock_initialized_ = false;
  clock_start_media_us_ = 0;
}

void FrameScheduler::initClockIfNeeded() {
  if (clock_initialized_) {
    return;
  }

  // 找到所有轨道队头中最早的媒体时间，作为媒体时间起点
  bool found_any = false;
  int64_t min_pts_us = 0;

  for (auto &[id, ts] : tracks_) {
    if (ts.queue.empty()) {
      continue;
    }

    const MediaFrame &front = ts.queue.front();
    int64_t pts_us = toGlobalTimeUs(front.pts, front.time_base);
    if (!found_any || pts_us < min_pts_us) {
      min_pts_us = pts_us;
      found_any = true;
    }
  }

  // 如果没有任何帧，就从 0 开始
  if (!found_any) {
    min_pts_us = 0;
  }

  clock_start_wall_ = SteadyClock::now();
  clock_start_media_us_ = min_pts_us;
  clock_initialized_ = true;
}

// 在所有轨道中选择“媒体时间最早且已到时间”的帧
bool FrameScheduler::selectCandidateFrame(int64_t now_media_us,
                                          int &out_track_id,
                                          MediaFrame &out_frame,
                                          int64_t &out_frame_pts_us) {
  bool found_candidate = false;
  int64_t candidate_pts_us = 0;
  int candidate_track_id = -1;
  MediaFrame candidate_frame{};

  for (auto &[id, ts] : tracks_) {
    if (ts.queue.empty()) {
      continue;
    }

    const MediaFrame &front = ts.queue.front();
    int64_t pts_us = toGlobalTimeUs(front.pts, front.time_base);

    // 如果这帧对应的媒体时间还没到当前播放时间，则不能发送
    if (pts_us > now_media_us) {
      continue;
    }

    if (!found_candidate || pts_us < candidate_pts_us) {
      found_candidate = true;
      candidate_pts_us = pts_us;
      candidate_track_id = id;
      candidate_frame = front;
    }
  }

  if (!found_candidate) {
    return false;
  }

  out_track_id = candidate_track_id;
  out_frame = candidate_frame;
  out_frame_pts_us = candidate_pts_us;
  return true;
}

bool FrameScheduler::pollReadyFrame(ScheduledFrame &out) {
  if (playback_.state != PlayState::Playing) {
    return false;
  }

  if (tracks_.empty()) {
    return false;
  }

  // 如果还没有初始化时钟，在第一次 play 时会初始化
  if (!clock_initialized_) {
    initClockIfNeeded();
  }

  int64_t now_wall_us = nowWallTimeUs();
  int64_t delta_us =
      now_wall_us - std::chrono::duration_cast<std::chrono::microseconds>(
                        clock_start_wall_.time_since_epoch())
                        .count();

  // 媒体时间 = 起点媒体时间 + 实际经过时间 * 播放速率
  long double media_time_ld =
      static_cast<long double>(clock_start_media_us_) +
      static_cast<long double>(delta_us) * playback_.rate;
  int64_t now_media_us = static_cast<int64_t>(media_time_ld);

  int track_id = -1;
  MediaFrame selected_frame{};
  int64_t frame_pts_us = 0;

  if (!selectCandidateFrame(now_media_us, track_id, selected_frame,
                            frame_pts_us)) {
    return false;
  }

  // 从对应轨道队列中弹出该帧
  auto it = tracks_.find(track_id);
  if (it == tracks_.end() || it->second.queue.empty()) {
    return false;
  }
  it->second.queue.pop_front();

  it->second.last_sent_pts_us = frame_pts_us;
  it->second.last_sent_wall_us = now_wall_us;

  out.frame = selected_frame;
  out.send_time_us = now_wall_us;
  return true;
}

} // namespace synthesizer::media
