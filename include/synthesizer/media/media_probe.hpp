#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
namespace synthesizer::media {

class MediaProbe {
public:
  AVFormatContext av;
};

} // namespace synthesizer::media