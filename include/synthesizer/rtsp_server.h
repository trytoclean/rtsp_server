// include/synthesizer/synthesizer.h
#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace synthesizer {

struct ServerConfig {
  std::string bind_ip = "0.0.0.0";
  uint16_t port = 8554;
};

class RtspServer {
public:
  explicit RtspServer(ServerConfig config);
  ~RtspServer();

  RtspServer(const RtspServer &) = delete;
  RtspServer &operator=(const RtspServer &) = delete;

  bool start();
  void stop();

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace synthesizer
