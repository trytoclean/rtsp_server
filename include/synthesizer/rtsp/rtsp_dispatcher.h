#pragma once
#include <memory>
#include <synthesizer/rtsp/rtsp_handlers.h>
#include <synthesizer/rtsp/rtsp_parser.h>
namespace synthesizer::rtsp {
class RtspDispatcher {
public:
  RtspDispatcher();
  RtspResponse dispatch(const RtspRequest &req);
  RtspResponse onMessage(const std::string &raw);

private:
  std::unordered_map<Method, std::unique_ptr<IRtspHandler>> handlers_;
};
} // namespace synthesizer::rtsp