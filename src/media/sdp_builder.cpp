#include <sstream>
#include <synthesizer/media/sdp_builder.hpp>

namespace synthesizer::media {

std::string SDPBuilder::build(const MediaInfo &info, const std::string &url) {
  std::ostringstream sdp;

  // Session level lines
  sdp << "v=0\r\n";
  sdp << "o=- 0 0 IN IP4 127.0.0.1\r\n";
  sdp << "s=" << (info.sessionName.empty() ? "RTSP Session" : info.sessionName)
      << "\r\n";
  sdp << "t=0 0\r\n";

  // For each track
  for (const auto &track : info.tracks) {
    switch (track.codec) {
    case CodecType::H264:
      sdp << buildVideoH264(track, url);
      break;
    case CodecType::AAC:
      sdp << buildAudioAAC(track, url);
      break;
    default:
      break;
    }
  }

  return sdp.str();
}

// ---------------------------
//  VIDEO / H.264
// ---------------------------

std::string SDPBuilder::buildVideoH264(const TrackInfo &track,
                                       const std::string &url) {
  std::ostringstream out;

  out << "m=video 0 RTP/AVP " << track.payloadType << "\r\n";
  out << "c=IN IP4 0.0.0.0\r\n";
  out << "a=rtpmap:" << track.payloadType << " H264/90000\r\n";

  // a=fmtp line requires SPS/PPS in base64
  out << "a=fmtp:" << track.payloadType << " packetization-mode=1;"
      << "sprop-parameter-sets=" << track.spsBase64 << "," << track.ppsBase64
      << ";\r\n";

  // control line
  out << "a=control:" << url << "/track" << track.id << "\r\n";

  return out.str();
}

// ---------------------------
//  AUDIO / AAC (MPEG4-GENERIC)
// ---------------------------

std::string SDPBuilder::buildAudioAAC(const TrackInfo &track,
                                      const std::string &url) {
  std::ostringstream out;

  out << "m=audio 0 RTP/AVP " << track.payloadType << "\r\n";
  out << "c=IN IP4 0.0.0.0\r\n";
  out << "a=rtpmap:" << track.payloadType << " MPEG4-GENERIC/"
      << track.sampleRate << "/" << track.channels << "\r\n";

  out << "a=fmtp:" << track.payloadType
      << " streamtype=5; profile-level-id=1; mode=AAC-hbr;"
      << " config=1210;" // MVP 固定值，可从 AudioSpecificConfig 计算
      << " SizeLength=13; IndexLength=3; IndexDeltaLength=3;\r\n";

  out << "a=control:" << url << "/track" << track.id << "\r\n";

  return out.str();
}

} // namespace synthesizer::media
