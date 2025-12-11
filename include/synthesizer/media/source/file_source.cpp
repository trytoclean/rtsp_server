#include "file_source.hpp"

// FFmpeg 头文件
extern "C" {
#include <libavformat/avformat.h>
}

namespace synthesizer::media {

FileSource::FileSource(std::string path) : path_(std::move(path)) {}

FileSource::~FileSource() { close(); }

bool FileSource::open() {
  // 打开文件
  if (avformat_open_input(&fmt_, path_.c_str(), nullptr, nullptr) < 0) {
    return false;
  }

  // 读取流信息
  if (avformat_find_stream_info(fmt_, nullptr) < 0) {
    return false;
  }

  // 填充 MediaInfo
  info_.duration = fmt_->duration;

  for (unsigned i = 0; i < fmt_->nb_streams; i++) {
    auto *st = fmt_->streams[i];
    TrackInfo ti;
    ti.id = i;

    switch (st->codecpar->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
      ti.codec = CodecType::H264;
      break;
    case AVMEDIA_TYPE_AUDIO:
      ti.codec = CodecType::AAC;
      break;
    default:
      ti.codec = CodecType::UNKNOWN;
    }

    // extradata
    ti.extra.assign(st->codecpar->extradata,
                    st->codecpar->extradata + st->codecpar->extradata_size);

    info_.tracks.push_back(std::move(ti));
  }

  return true;
}

void FileSource::close() {
  if (fmt_) {
    avformat_close_input(&fmt_);
    fmt_ = nullptr;
  }
}

const MediaInfo &FileSource::getMediaInfo() const { return info_; }

bool FileSource::readFrame(MediaFrame &frame) {
  AVPacket pkt;

  if (av_read_frame(fmt_, &pkt) < 0) {
    return false; // EOF or error
  }

  // 填充 MediaFrame
  frame.trackId = pkt.stream_index;
  frame.pts = pkt.pts;
  frame.dts = pkt.dts;
  frame.data.assign(pkt.data, pkt.data + pkt.size);

  // codec
  if (fmt_->streams[pkt.stream_index]->codecpar->codec_type ==
      AVMEDIA_TYPE_VIDEO)
    frame.codec = CodecType::H264;
  else if (fmt_->streams[pkt.stream_index]->codecpar->codec_type ==
           AVMEDIA_TYPE_AUDIO)
    frame.codec = CodecType::AAC;

  av_packet_unref(&pkt);
  return true;
}

} // namespace synthesizer::media
