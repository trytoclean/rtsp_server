#pragma once
#include "session_info.hpp"
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace synthesizer::rtsp {

class SessionManager {
public:
  SessionManager() = default;

  // 创建新的 RTSP 会话
  std::shared_ptr<SessionInfo> createSession(int timeout = 60);

  // 通过 session-id 查找 session
  std::shared_ptr<SessionInfo> getSession(const std::string &id);

  // 删除会话
  void removeSession(const std::string &id);

  // 检查并清理已超时的会话（需周期调用）
  void cleanupExpired();

private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<SessionInfo>> sessions_;
};

} // namespace synthesizer::rtsp
