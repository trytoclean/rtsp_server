#pragma once
#include <stdint.h>
#include <string>
#include <vector>
extern "C" {

#include <libavformat/avformat.h>
}
namespace synthesizer::media {

enum class CodecType { H264, AAC, UNKNOWN };

struct TrackInfo {
  int id = -1; // track 编号（媒体容器中的 index）
  CodecType codec = CodecType::UNKNOWN;
  std::string codecName; // “H264”“AAC”等

  // ========== codec extra 信息 ==========
  std::vector<uint8_t> extra; // codec extra data（H264 SPS/PPS 或 AAC ASC）

  // ========== RTP 相关 ==========
  int payloadType = 96;  // RTP PT，动态类型一般从 96 开始
  int clockRate = 90000; // H264 始终为 90000，AAC 为采样率
  std::string fmtp;      // 用于直接放进 SDP，可选

  // ========== 视频属性 ==========
  int width = 0;
  int height = 0;
  float fps = 0.0f; // 可选，但对 scheduler 有意义

  // ========== 音频属性 ==========
  int sampleRate = 0;
  int channels = 0;
  int bitsPerSample = 0; // optional

  // ========== 时间基 ==========
  AVRational timebase = {1, 1000}; // 来自 FFmpeg；PTS/DTS normalization 使用

  // ========== H264/H265 特定字段 ==========
  std::string spsBase64;
  std::string ppsBase64;
  std::string vpsBase64; // For HEVC

  // ========== RTSP control path ==========
  std::string control; // SDP 的 a=control:/trackID
};

struct MediaInfo {
  std::string sessionName; // SDP s=<session-name>
  int64_t duration = 0;    // 微秒；用于 DESCRIBE Range 信息
  bool isLive = false;     // live stream 没有 duration

  AVRational globalTimebase = {1, 1000}; // 全局时间基（可选，通常由视频轨决定）

  int videoTrackIndex = -1; // 默认视频轨
  int audioTrackIndex = -1; // 默认音频轨

  std::vector<TrackInfo> tracks; // 轨道列表

  // v2,v3
  int64_t bitrate = 0; // 码率；可用于统计
  int rotation = 0;    // 视频旋转（移动设备录制常见）
};

struct MediaFrame {
  std::vector<uint8_t> data;
  int64_t pts = 0;
  int64_t dts = 0;
  int trackId = -1;
  CodecType codec = CodecType::UNKNOWN;
};

class IMediaSource {
public:
  virtual ~IMediaSource() = default;

  virtual bool open() = 0;
  virtual void close() = 0;

  // 返回媒体基本信息（仅读，不复制）
  virtual const MediaInfo &getMediaInfo() const = 0;

  // 读取下一帧（compressed frame）
  virtual bool readFrame(MediaFrame &frame) = 0;
};

} // namespace synthesizer::media
