// src/main.cc
#include "synthesizer/synthesizer.h"

int main() {
  synthesizer::ServerConfig config;
  config.bind_ip = "0.0.0.0";
  config.port = 8554;

  synthesizer::RtspServer server(config);
  if (!server.start()) {
    return 1;
  }
  return 0;
}
