#include "../irtsp_handler.hpp"

namespace synthesizer::rtsp {

class PlayHandler : public IRtspHandler {

  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp