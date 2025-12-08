#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace synthesizer::media {

enum class MediaType { Video, Audio, Unknown };

struct TimeBase {
  int num{0}; // 分子
  int den{1}; // 分母，必须非 0
};

// Demux 输出给调度器的媒体帧
struct MediaFrame {
  int track_id{0}; // 轨道 ID（视频/音频等）
  MediaType type{MediaType::Unknown};

  int64_t pts{0};      // 显示时间戳，单位：time_base
  int64_t dts{0};      // 解码时间戳，单位：time_base
  int64_t duration{0}; // 帧时长（可选），单位：time_base
  bool key_frame{false};

  TimeBase time_base{1, 1}; // 本轨道时间基

  const uint8_t *data{
      nullptr}; // 数据指针（MVP 版只保存裸指针，不做所有权管理）
  int size{0}; // 数据长度
};

// 调度器对外输出的“就绪帧”
struct ScheduledFrame {
  MediaFrame frame;     // 媒体帧
  int64_t send_time_us; // 建议发送时刻（wall time，微秒）
};

enum class PlayState { Stopped, Playing, Paused };

class FrameScheduler {
public:
  // global_time_base 默认为 1/1_000_000 秒，即微秒
  explicit FrameScheduler(TimeBase global_time_base = TimeBase{1, 1000000});

  // 喂帧（通常由 Demux 调用）
  void pushFrame(const MediaFrame &frame);

  // 拉取就绪帧（通常由 RTP Builder 调用）
  // 返回 true 表示 out 填充了一帧；返回 false 表示当前没有可发送帧
  bool pollReadyFrame(ScheduledFrame &out);

  // 播放控制
  void play();
  void pause();
  void stop();
  void setRate(double rate); // 1.0 正常倍速

  // 重置调度器状态（清空所有队列和时钟）
  void reset();

private:
  struct TrackState {
    int track_id{0};
    MediaType type{MediaType::Unknown};
    TimeBase time_base{1, 1}; // 本轨道时间基

    std::deque<MediaFrame> queue; // 此轨道的待发送帧队列

    int64_t last_sent_pts_us{0}; // 上一次发送的媒体时间（统一时间轴）
    int64_t last_sent_wall_us{0}; // 上一次发送的 wall time（微秒）
  };

  struct PlaybackState {
    PlayState state{PlayState::Stopped};
    double rate{1.0}; // 播放速率
  };

  using SteadyClock = std::chrono::steady_clock;

  // 内部时间帮助函数
  int64_t nowWallTimeUs() const;
  int64_t toGlobalTimeUs(int64_t ts, const TimeBase &src_tb) const;

  // 初始化媒体时钟（第一次开始播放时调用）
  void initClockIfNeeded();

  // 在所有轨道的队头中寻找“媒体时间最早且已经到时间”的那一帧
  bool selectCandidateFrame(int64_t now_media_us, int &out_track_id,
                            MediaFrame &out_frame, int64_t &out_frame_pts_us);

private:
  TimeBase global_time_base_; // 统一时间基（例如 1/1_000_000 秒）

  PlaybackState playback_; // 播放状态

  bool clock_initialized_{false};
  SteadyClock::time_point clock_start_wall_; // wall time 起点
  int64_t clock_start_media_us_{0}; // 媒体时间起点（统一时间轴）

  std::unordered_map<int, TrackState> tracks_; // 所有轨道状态，键为 track_id
};

} // namespace synthesizer::media
