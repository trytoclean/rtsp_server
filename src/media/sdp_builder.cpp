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

/* sdp in real
Session Description Protocol
    Session Description Protocol Version (v): 0
    Owner/Creator, Session Id (o): - 1758013524560723 1 IN IP4 192.168.239.129
    Session Name (s): Matroska video+audio+(optional)subtitles, streamed by the
LIVE555 Media Server Session Information (i): test.mkv Time Description, active
time (t): 0 0 Session Attribute (a): tool:LIVE555 Streaming Media v2025.07.19
    Session Attribute (a): type:broadcast
    Session Attribute (a): control:*
    Session Attribute (a): range:npt=0-201.915
    Session Attribute (a): x-qt-text-nam:Matroska
video+audio+(optional)subtitles, streamed by the LIVE555 Media Server Session
Attribute (a): x-qt-text-inf:test.mkv Media Description, name and address (m):
video 0 RTP/AVP 96 Connection Information (c): IN IP4 0.0.0.0 Bandwidth
Information (b): AS:500 Media Attribute (a): rtpmap:96 H264/90000 Media
Attribute (a): fmtp:96
packetization-mode=1;profile-level-id=64001F;sprop-parameter-sets=Z2QAH6zZQNg95+EAAAMAAQAAAwA8Dxgxlg==,aOvjyyLA
    Media Attribute (a): control:track1
    Media Description, name and address (m): audio 0 RTP/AVP 97
    Connection Information (c): IN IP4 0.0.0.0
    Bandwidth Information (b): AS:112
    Media Attribute (a): rtpmap:97 VORBIS/48000/2
    Media Attribute (a) […]: fmtp:97


*/