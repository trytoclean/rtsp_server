#include "../irtsp_handler.hpp"

namespace synthesizer::rtsp {

class SetupHandler : public IRtspHandler {

  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp