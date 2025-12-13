#include "../irtsp_handler.hpp"
#include "../session_manager.hpp"
namespace synthesizer::rtsp {

class PlayHandler : public IRtspHandler {
public:
  explicit PlayHandler(std::shared_ptr<SessionManager> session_manager);

private:
  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp