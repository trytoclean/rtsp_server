#pragma once
#include "handlers/describe_handler.hpp"
#include "handlers/options_handler.hpp"
#include "handlers/pause_handler.hpp"
#include "handlers/play_handler.hpp"
#include "handlers/setup_handler.hpp"
#include "handlers/teardown_handler.hpp"
#include "rtsp_parser.hpp"
#include "session_manager.hpp"
#include <memory>

namespace synthesizer::rtsp {
class RtspDispatcher {
public:
  RtspDispatcher();
  RtspResponse dispatch(const RtspRequest &req);
  ~RtspDispatcher();

private:
  std::unordered_map<Method, std::unique_ptr<IRtspHandler>> handlers_;
  std::shared_ptr<SessionManager> session_manager_;
};

} // namespace synthesizer::rtsp