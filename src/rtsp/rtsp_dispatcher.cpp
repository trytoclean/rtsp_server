#include <memory>
#include <synthesizer/rtsp/rtsp_dispatcher.hpp>
#include <synthesizer/rtsp/rtsp_parser.hpp>
#include <synthesizer/rtsp/session_manager.hpp>
namespace synthesizer::rtsp {
RtspDispatcher::RtspDispatcher()
    : session_manager_(std::make_shared<SessionManager>()) {

  handlers_[Method::OPTIONS] = std::make_unique<OptionsHandler>();
  handlers_[Method::DESCRIBE] = std::make_unique<DescribeHandler>();

  handlers_[Method::SETUP] = std::make_unique<SetupHandler>(session_manager_);

  handlers_[Method::PLAY] = std::make_unique<PlayHandler>(session_manager_);

  handlers_[Method::PAUSE] = std::make_unique<PauseHandler>(session_manager_);

  handlers_[Method::TEARDOWN] =
      std::make_unique<TeardownHandler>(session_manager_);
}

RtspDispatcher::~RtspDispatcher() = default;

RtspResponse RtspDispatcher::dispatch(const RtspRequest &req) {
  auto it = handlers_.find(req.method);
  if (it == handlers_.end())
    return RtspResponseBuilder::buildError(req, 501, "Not Implemented");

  return it->second->handle(req);
}
} // namespace synthesizer::rtsp
