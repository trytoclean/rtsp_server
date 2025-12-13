#pragma once
#include <ctime>
#include <string>
#include <unordered_map>

namespace synthesizer::rtsp {

struct TransportContext {
  bool unicast = true;
  std::string lowerTransport = "UDP";
  int clientRtpPort = -1;
  int clientRtcpPort = -1;
  int serverRtpPort = -1;
  int serverRtcpPort = -1;
};

enum class SessionState { INIT, READY, PLAYING, PAUSED };

struct SessionInfo {
  std::string id;
  time_t lastActive = 0;
  int timeout = 60;

  SessionState state = SessionState::INIT;

  // track_id → TransportContext
  std::unordered_map<int, TransportContext> transports;
};

inline std::string generateSessionId() {
  static const char hex[] = "0123456789ABCDEF";
  std::string id(16, '0');
  for (int i = 0; i < 16; i++) {
    id[i] = hex[rand() % 16];
  }
  return id;
}

} // namespace synthesizer::rtsp
