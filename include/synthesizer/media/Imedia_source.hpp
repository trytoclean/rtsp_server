#pragma once
#include "media_type.hpp"
extern "C" {
#include <libavformat/avformat.h>
}

#include <string>
#include <vector>

namespace synthesizer::media {
struct MediaFrame {
  int track_id;              // 属于哪条轨道
  std::vector<uint8_t> data; // 编码后的数据（NALU, AAC ES）
  int64_t pts;               // 显示时间戳
  int64_t dts;               // 解码时间戳
  bool keyframe;             // 是否关键帧
};
struct TrackInfo {
  int track_id;                   // FFmpeg中的stream index
  CodecType avcodec;              // H264 / AAC
  AVRational timebase;            // 时间基，例如 {1,90000}
  int width, height;              // 视频
  int sample_rate;                // 音频
  std::vector<uint8_t> extradata; // SPS/PPS 或 AAC AudioSpecificConfig
};

struct MediaInfo {
  std::string format;            // 容器格式，如 mp4, flv
  std::vector<TrackInfo> tracks; // 所有轨道的元信息
  // bool is_live;                  // 是否为直播源
};

class IMediaSource {

public:
  virtual ~IMediaSource() = default;
  virtual bool open(const std::string &url) = 0;
  virtual MediaInfo probe() = 0;
  virtual bool readFrame(MediaFrame &frame) = 0;
  virtual void close() = 0;
};
} // namespace synthesizer::media