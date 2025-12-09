#pragma once
#include "synthesizer/net/inet_address.hpp"
#include <functional>
#include <synthesizer/net/socket.hpp>
namespace synthesizer::net {

class Acceptor {
public:
  using NewConnCallback = std::function<void(int)>;

  Acceptor(int port);
  ~Acceptor();

  void setNewConnCallback(NewConnCallback cb) { callback_ = std::move(cb); }

  int fd() const { return listen_fd_; }

  // 供 Epoll 触发 EPOLLIN 时调用
  void handleAccept();

private:
  int listen_fd_;
  NewConnCallback callback_;
  InetAddress addr;
};

} // namespace synthesizer::net
