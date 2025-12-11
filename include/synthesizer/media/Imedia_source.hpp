#pragma once
#include "media_frame.hpp"
#include "media_metadata.hpp"
#include <stdint.h>
extern "C" {

#include <libavformat/avformat.h>
}
namespace synthesizer::media {

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
