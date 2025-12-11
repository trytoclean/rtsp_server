#pragma once
#include "../Imedia_source.hpp"
#include <string>

struct AVFormatContext; // FFmpeg 前向声明
struct AVPacket;

namespace synthesizer::media {

class FileSource : public IMediaSource {
public:
  explicit FileSource(std::string path);
  ~FileSource();

  bool open() override;
  void close() override;

  const MediaInfo &getMediaInfo() const override;
  bool readFrame(MediaFrame &frame) override;

private:
  std::string path_;
  MediaInfo info_;

  AVFormatContext *fmt_ = nullptr;
};

} // namespace synthesizer::media
