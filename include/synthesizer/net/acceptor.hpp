/**
 * @file acceptor.hpp
 * @brief 创建acceptor对象，接受客户端连接
 *
 * 职责边界：
 * 1) 不做业务决策，只做路由
 * 2) 不持有 socket / 不做网络 I/O
 *
 * 典型调用：
 *  Accepetor(int port);
 *
 * @author trytoclean
 * @date 2025-11-23
 */
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
  InetAddress addr_;
};

} // namespace synthesizer::net
