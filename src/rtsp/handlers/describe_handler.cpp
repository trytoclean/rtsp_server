#include <synthesizer/rtsp/handlers/options_handler.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>

namespace synthesizer::rtsp {
RtspResponse handle(const RtspRequest &req) {
  RtspResponse res = RtspResponseBuilder::buildSuccess(req);
  res.headers["Public"] = "OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN";
  res.headers["Content-Type"] = "application/sdp";
  res.headers["Content-Length"] = std::to_string(res.body.size());
  return res;
}
} // namespace synthesizer::rtsp

/* 节选自 live555的真实报文：SDP
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

/* reply：
Real Time Streaming Protocol
    Response: RTSP/1.0 200 OK\r\n
    CSeq: 2
    Date: Tue, Sep 16 2025 09:05:24 GMT\r\n
    Content-Base: rtsp://192.168.239.129:8554/test.mkv/
    Content-type: application/sdp
    Content-length: 6002  <- 描述SDP长度的. 如果缺少此字段，则默认值为零。>=0
vaild \r\n Session Description Protocol
*/
