/**
 * @file socket.hpp
 * @brief 分配socket套接字、绑定监听
 *
 * 职责边界：
 * 1) 不做业务决策，只做路由
 * 2) 不持有 socket / 不做网络 I/O
 *
 * 典型调用：
 *   RtspResponse res = dispatcher.dispatch(req);
 *
 * @author trytoclean
 * @date 2025-11-23
 */
#pragma once
#include "inet_address.hpp"

namespace synthesizer::net {
class Socket {
public:
  // 创建 socket(AF_INET, SOCK_STREAM)
  Socket();
  // 接受 accept() 返回的 fd
  explicit Socket(int fd);
  ~Socket();
  int fd() const { return fd_; }
  void bindSocket(const InetAddress &addr);
  void listenSocket(int backlog = 1024);
  void setNonBlocking();

private:
  int fd_{-1};
};

} // namespace synthesizer::net
