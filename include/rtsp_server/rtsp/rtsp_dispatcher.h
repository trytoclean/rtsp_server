#pragma once
#include <memory>
#include <rtsp_server/rtsp/rtsp_handlers.h>
#include <rtsp_server/rtsp/rtsp_parser.h>
namespace rtsp_server::rtsp {
class RtspDispatcher {
public:
  RtspDispatcher();
  RtspResponse dispatch(const RtspRequest &req);
  RtspResponse onMessage(const std::string &raw);

private:
  std::unordered_map<Method, std::unique_ptr<IRtspHandler>> handlers_;
};
} // namespace rtsp_server::rtsp