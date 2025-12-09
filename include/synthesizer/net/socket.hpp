#pragma once
#include "inet_address.hpp"

namespace synthesizer::net {

class Socket {
public:
  Socket();                // 创建 socket(AF_INET, SOCK_STREAM)
  explicit Socket(int fd); // 接受 accept() 返回的 fd
  ~Socket();

  int fd() const { return fd_; }

  void bindSocket(const InetAddress &addr);
  void listenSocket(int backlog = 1024);
  void setNonBlocking();

private:
  int fd_{-1};
};

} // namespace synthesizer::net
