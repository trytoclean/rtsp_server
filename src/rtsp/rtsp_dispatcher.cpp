#include <memory>
#include <synthesizer/rtsp/rtsp_dispatcher.hpp>
#include <synthesizer/rtsp/rtsp_parser.hpp>

namespace synthesizer::rtsp {

RtspDispatcher::RtspDispatcher() {
  handlers[Method::OPTIONS] = std::make_unique<OptionsHandler>();
  handlers[Method::SETUP] = std::make_unique<SetupHandler>();
  handlers[Method::DESCRIBE] = std::make_unique<DescribeHandler>();
  handlers[Method::PLAY] = std::make_unique<PlayHandler>();
  handlers[Method::TEARDOWN] = std::make_unique<TeardownHandler>();
  handlers[Method::PAUSE] = std::make_unique<PauseHandler>();
}
RtspDispatcher::~RtspDispatcher() = default;
RtspResponse RtspDispatcher::dispatch(const RtspRequest &req) {
  auto it = handlers.find(req.method);
  if (it == handlers.end())
    return RtspResponseBuilder::buildError(req, 501, "Not Implemented");

  return it->second->handle(req);
}
} // namespace synthesizer::rtsp
