#include "synthesizer/rtsp/session_info.hpp"
#include <ctime>
#include <synthesizer/rtsp/session_manager.hpp>

namespace synthesizer::rtsp {

std::shared_ptr<SessionInfo> SessionManager::createSession(int timeout) {
  std::lock_guard<std::mutex> lock(mutex_);

  std::string id = generateSessionId();

  auto session = std::make_shared<SessionInfo>();
  session->id = id;
  session->timeout = timeout;
  session->lastActive = std::time(nullptr);
  session->state = SessionState::INIT;

  sessions_[id] = session;
  return session;
}

std::shared_ptr<SessionInfo> SessionManager::getSession(const std::string &id) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = sessions_.find(id);
  if (it != sessions_.end()) {
    return it->second;
  }
  return nullptr;
}

void SessionManager::removeSession(const std::string &id) {
  std::lock_guard<std::mutex> lock(mutex_);
  sessions_.erase(id);
}

void SessionManager::cleanupExpired() {
  std::lock_guard<std::mutex> lock(mutex_);
  time_t now = std::time(nullptr);

  for (auto it = sessions_.begin(); it != sessions_.end();) {
    auto &session = it->second;
    if (now - session->lastActive > session->timeout) {
      it = sessions_.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace synthesizer::rtsp
