#include "../irtsp_handler.hpp"
#include "synthesizer/rtsp/session_manager.hpp"
#include <memory>

namespace synthesizer::rtsp {

class TeardownHandler : public IRtspHandler {
public:
  TeardownHandler(std::shared_ptr<SessionManager> handlers_);

private:
  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp