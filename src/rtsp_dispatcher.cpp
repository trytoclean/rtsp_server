#include "rtsp_server/rtsp/rtsp_dispatcher.h"
rtsp_server::rtsp::RtspDispatcher::RtspDispatcher() {
  handlers_[Method::OPTIONS] = std::make_unique<OptionsHandler>();
}

RtspResponse
rtsp_server::rtsp::RtspDispatcher::dispatch(const RtspRequest &req) {
  auto it = handlers_.find(req.method);
  if (it == handlers_.end()) {
    return RtspResponseBuilder::error(req, 405, "Method Not Allowed");
  }
  return it->second->handle(req);
}