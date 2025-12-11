#include <string>
#include <synthesizer/rtsp/handlers/options_handler.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>

namespace synthesizer::rtsp {
RtspResponse handle(const RtspRequest &req) {
  RtspResponse res = RtspResponseBuilder::buildSuccess(req);
  res.setHeader("Public", "OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN");
  res.setHeader("Content-Type", "application/sdp");
  res.setHeader("Content-Length", std::to_string(res.body.size()));
  return res;
}
} // namespace synthesizer::rtsp