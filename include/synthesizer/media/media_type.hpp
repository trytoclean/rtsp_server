#pragma once
#include <string>
namespace synthesizer::media {
enum CodecType {
  UNKNOWN = 0,
  H264,
  AAC

};
inline std::string CodecTypeToString(CodecType type) {
  switch (type) {
  case CodecType::H264:
    return "H264";
  case CodecType::AAC:
    return "AAC";
  case CodecType::UNKNOWN:
    return "UNKOWN";
  }
}

} // namespace synthesizer::media