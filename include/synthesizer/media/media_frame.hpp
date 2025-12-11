#pragma once
#include "codec_type.hpp"
#include <stdint.h>
#include <vector>
namespace synthesizer::media {

struct MediaFrame {
  std::vector<uint8_t> data;
  int64_t pts = 0;
  int64_t dts = 0;
  int trackId = -1;
  CodecType codec = CodecType::UNKNOWN;
};
} // namespace synthesizer::media