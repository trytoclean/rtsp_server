#pragma once
#include <string>
#include <synthesizer/media/Imedia_source.hpp>

namespace synthesizer::media {

class SDPBuilder {
public:
  static std::string build(const MediaInfo &info, const std::string &url);

private:
  static std::string buildVideoH264(const TrackInfo &track,
                                    const std::string &url);
  static std::string buildAudioAAC(const TrackInfo &track,
                                   const std::string &url);
};

} // namespace synthesizer::media
