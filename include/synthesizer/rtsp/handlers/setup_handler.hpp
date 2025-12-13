#include "../irtsp_handler.hpp"
#include "../session_manager.hpp"

namespace synthesizer::rtsp {

class SetupHandler : public IRtspHandler {
public:
  explicit SetupHandler(std::shared_ptr<SessionManager> session_manager);

private:
  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp