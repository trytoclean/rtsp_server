#include <synthesizer/rtsp/rtsp_handler.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>

namespace synthesizer::rtsp {
class OptionsHandler : public IRtspHandler {
  RtspResponse handle(const RtspRequest &req) override {
    RtspResponse res = RtspResponseBuilder::buildSuccess(req);
    res.headers["Public"] = "OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN";
    return res;
  }
};
} // namespace synthesizer::rtsp