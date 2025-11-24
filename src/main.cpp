// src/main.cc
#include "rtsp_server/rtsp_server.h"

int main() {
  rtsp_server::ServerConfig config;
  config.bind_ip = "0.0.0.0";
  config.port = 8554;

  rtsp_server::RtspServer server(config);
  if (!server.start()) {
    return 1;
  }
  return 0;
}
