// file_media_source.hpp
#pragma once
#include <string>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/base64.h>
}
#include "../Imedia_source.hpp"

namespace synthesizer::media {

class FileSource : public IMediaSource {
public:
  explicit FileSource(std::string path);
  ~FileSource() override;

  bool open() override;
  void close() override;
  bool readFrame(MediaFrame &frame) override;
  const MediaInfo &getMediaInfo() const override { return info_; }

private:
  bool buildMediaInfo(); // 核心：从 AVFormatContext 构建 MediaInfo

  TrackInfo buildTrackFromStream(AVStream *st, int trackId,
                                 int payloadTypeBase);

  void fillH264Track(TrackInfo &ti, const AVCodecParameters *codecpar);
  void fillAACTrack(TrackInfo &ti, const AVCodecParameters *codecpar);

private:
  std::string path_;
  AVFormatContext *fmt_ = nullptr;
  MediaInfo info_;
};

} // namespace synthesizer::media
