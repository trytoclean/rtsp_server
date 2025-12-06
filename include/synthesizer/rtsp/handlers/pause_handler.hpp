#include "../irtsp_handler.hpp"

namespace synthesizer::rtsp {

class PauseHandler : public IRtspHandler {

  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp