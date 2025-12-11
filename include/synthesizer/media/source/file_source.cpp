#include "file_source.hpp"
#include <iomanip>
#include <sstream>
// FFmpeg 头文件
extern "C" {
#include <libavformat/avformat.h>
}
namespace {

// 简单的 AVCDecoderConfigurationRecord 解析，只提取第一个 SPS/PPS
bool parseAvcCExtradataSpsPps(const uint8_t *data, int size,
                              std::string &spsOut, std::string &ppsOut) {
  if (!data || size < 7)
    return false;

  int pos = 0;
  uint8_t configurationVersion = data[pos++];
  uint8_t avcProfileIndication = data[pos++];
  uint8_t profile_compatibility = data[pos++];
  uint8_t avcLevelIndication = data[pos++];
  uint8_t lengthSizeMinusOne = data[pos++];

  (void)configurationVersion;
  (void)avcProfileIndication;
  (void)profile_compatibility;
  (void)avcLevelIndication;
  (void)lengthSizeMinusOne;

  uint8_t numOfSPS = data[pos++] & 0x1F;
  if (numOfSPS == 0)
    return false;

  // 读第一个 SPS
  if (pos + 2 > size)
    return false;
  int spsLen = (data[pos] << 8) | data[pos + 1];
  pos += 2;
  if (pos + spsLen > size)
    return false;
  const uint8_t *sps = data + pos;
  pos += spsLen;

  // 读 PPS 数量
  if (pos + 1 > size)
    return false;
  uint8_t numOfPPS = data[pos++];
  if (numOfPPS == 0)
    return false;

  // 读第一个 PPS
  if (pos + 2 > size)
    return false;
  int ppsLen = (data[pos] << 8) | data[pos + 1];
  pos += 2;
  if (pos + ppsLen > size)
    return false;
  const uint8_t *pps = data + pos;
  pos += ppsLen;

  // 使用 av_base64_encode 将 raw SPS/PPS 转为 Base64
  auto toBase64 = [](const uint8_t *buf, int len) -> std::string {
    if (!buf || len <= 0)
      return {};

    int outSize = AV_BASE64_SIZE(len);
    std::string out(outSize, '\0');

    auto ret = av_base64_encode(out.data(), outSize, buf, len);
    (void)ret; // 不依赖返回值，兼容 int / char*

    out.resize(strlen(out.c_str()));
    return out;
  };

  spsOut = toBase64(sps, spsLen);
  ppsOut = toBase64(pps, ppsLen);
  return !spsOut.empty() && !ppsOut.empty();
}

// AAC AudioSpecificConfig 转 16 进制字符串
std::string bytesToHex(const uint8_t *data, int size) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (int i = 0; i < size; ++i) {
    oss << std::setw(2) << static_cast<int>(data[i]);
  }
  return oss.str();
}

} // namespace

namespace synthesizer::media {
FileSource::FileSource(std::string path) : path_(std::move(path)) {}

FileSource::~FileSource() { close(); }

bool FileSource::open() {
  if (fmt_) {
    return true; // already opened
  }

  AVFormatContext *ctx = nullptr;
  if (avformat_open_input(&ctx, path_.c_str(), nullptr, nullptr) < 0) {
    return false;
  }

  if (avformat_find_stream_info(ctx, nullptr) < 0) {
    avformat_close_input(&ctx);
    return false;
  }

  fmt_ = ctx;

  if (!buildMediaInfo()) {
    close();
    return false;
  }

  return true;
}

void FileSource::close() {
  if (fmt_) {
    avformat_close_input(&fmt_);
    fmt_ = nullptr;
  }
}
bool FileSource::buildMediaInfo() {
  if (!fmt_)
    return false;

  MediaInfo info;

  // 1) session 名字用文件名
  info.sessionName = path_; // MVP：简单用路径，以后可截取 basename

  // 2) duration（微秒）
  if (fmt_->duration > 0) {
    info.duration = fmt_->duration; // FFmpeg 本身就是 AV_TIME_BASE（一秒=1e6）
    info.isLive = false;
  } else {
    info.duration = 0;
    info.isLive = true; // 没有 duration，视作 LIVE
  }

  // 3) 遍历每条 AVStream，构建 TrackInfo
  int payloadBase = 96; // RTP 动态 PT 起始
  int videoTrackIdx = -1;
  int audioTrackIdx = -1;

  for (unsigned int i = 0; i < fmt_->nb_streams; ++i) {
    AVStream *st = fmt_->streams[i];
    if (!st || !st->codecpar)
      continue;

    TrackInfo ti = buildTrackFromStream(st, static_cast<int>(i), payloadBase);

    if (ti.codec == CodecType::UNKNOWN) {
      continue;
    }

    // 设置 video/audio index（tracks 中的下标）
    if (ti.codec == CodecType::H264 && videoTrackIdx == -1) {
      videoTrackIdx = static_cast<int>(info.tracks.size());
      info.globalTimebase = st->time_base; // 用视频轨 timebase 作为全局
    }
    if (ti.codec == CodecType::AAC && audioTrackIdx == -1) {
      audioTrackIdx = static_cast<int>(info.tracks.size());
    }

    payloadBase++; // 每个 track 分配一个 PT
    info.tracks.push_back(std::move(ti));
  }

  info.videoTrackIndex = videoTrackIdx;
  info.audioTrackIndex = audioTrackIdx;

  if (info.tracks.empty()) {
    return false; // 没找到任何可用媒体轨
  }

  info_ = std::move(info);
  return true;
}
TrackInfo FileSource::buildTrackFromStream(AVStream *st, int trackId,
                                           int payloadTypeBase) {
  TrackInfo ti;
  ti.id = trackId; // 直接用 AVStream::index 与 /track{id} 对应
  ti.timebase = st->time_base;

  const AVCodecParameters *codecpar = st->codecpar;
  ti.codecName = avcodec_get_name(codecpar->codec_id);

  // extradata 拷贝一份
  if (codecpar->extradata && codecpar->extradata_size > 0) {
    ti.extra.assign(codecpar->extradata,
                    codecpar->extradata + codecpar->extradata_size);
  }

  // 基础判断：视频 / 音频
  if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
    ti.width = codecpar->width;
    ti.height = codecpar->height;
    ti.clockRate = 90000; // H264/H265 统一 90k

    if (codecpar->codec_id == AV_CODEC_ID_H264) {
      ti.codec = CodecType::H264;
      ti.payloadType = payloadTypeBase; // 96, 97, ...
      fillH264Track(ti, codecpar);
    } else {
      ti.codec = CodecType::UNKNOWN;
    }
  } else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
    ti.sampleRate = codecpar->sample_rate;
    ti.channels = codecpar->channels;
    ti.clockRate = codecpar->sample_rate;

    if (codecpar->codec_id == AV_CODEC_ID_AAC) {
      ti.codec = CodecType::AAC;
      ti.payloadType = payloadTypeBase;
      fillAACTrack(ti, codecpar);
    } else {
      ti.codec = CodecType::UNKNOWN;
    }
  } else {
    ti.codec = CodecType::UNKNOWN;
  }

  // control 路径，由上层 SDPBuilder 再补 URL 前缀
  ti.control = "track" + std::to_string(ti.id);

  return ti;
}
void FileSource::fillH264Track(TrackInfo &ti,
                               const AVCodecParameters *codecpar) {
  if (!codecpar->extradata || codecpar->extradata_size <= 0) {
    return;
  }

  std::string spsB64, ppsB64;
  if (parseAvcCExtradataSpsPps(codecpar->extradata, codecpar->extradata_size,
                               spsB64, ppsB64)) {
    ti.spsBase64 = std::move(spsB64);
    ti.ppsBase64 = std::move(ppsB64);
  }

  // SDP fmtp 可以由 SDPBuilder 组合，也可以在这里预生成一部分
  std::ostringstream fmtp;
  fmtp << "packetization-mode=1";
  if (!ti.spsBase64.empty() && !ti.ppsBase64.empty()) {
    fmtp << ";sprop-parameter-sets=" << ti.spsBase64 << "," << ti.ppsBase64;
  }
  ti.fmtp = fmtp.str();
}
void FileSource::fillAACTrack(TrackInfo &ti,
                              const AVCodecParameters *codecpar) {
  if (codecpar->extradata && codecpar->extradata_size > 0) {
    ti.aacConfigHex = bytesToHex(codecpar->extradata, codecpar->extradata_size);
  }

  // clockRate 已在 buildTrackFromStream 设为 sampleRate

  // 这里构建一个 MVP 版 fmtp
  // 真正严格的 streamtype/profile 可以根据 ASC 解析，这里先固定常用值
  std::ostringstream fmtp;
  fmtp << "streamtype=5; profile-level-id=1; mode=AAC-hbr;";
  if (!ti.aacConfigHex.empty()) {
    fmtp << " config=" << ti.aacConfigHex << ";";
  } else {
    fmtp << " config=1210;"; // fallback（非常 MVP）
  }
  fmtp << " SizeLength=13; IndexLength=3; IndexDeltaLength=3;";
  ti.fmtp = fmtp.str();
}

} // namespace synthesizer::media

// namespace
