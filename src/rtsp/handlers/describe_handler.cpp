#include "synthesizer/media/media_metadata.hpp"
#include <string>
#include <synthesizer/media/media_source_manager.hpp>
#include <synthesizer/media/sdp_builder.hpp>
#include <synthesizer/media/source/file_source.hpp>
#include <synthesizer/rtsp/handlers/options_handler.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>
namespace synthesizer::rtsp {
RtspResponse handle(const RtspRequest &req) {
  RtspResponse res = RtspResponseBuilder::buildSuccess(req);
  res.headers["Content-Type"] = "application/sdp";
  res.headers["Content-Length"] = std::to_string(res.body.size());
  res.setHeader("Content-Base", req.uri);
  res.setHeader("Content-type", "application/sdp");
  media::MediaSourceManager manager(req.uri);
  auto source = manager.acquireSource(req.uri)->getMediaInfo();

  res.body = media::SDPBuilder::build(source, req.uri);
  res.setHeader("Content-length", std::to_string(req.body.size()));
  return res;
}
} // namespace synthesizer::rtsp

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
