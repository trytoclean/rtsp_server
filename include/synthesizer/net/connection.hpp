#pragma once
#include "buffer.hpp"
#include <functional>

namespace synthesizer::net {

class Epoll; // 前置声明

class Connection {
public:
  using MessageCallback =
      std::function<void(Connection *, const std::string &)>;
  using CloseCallback = std::function<void(Connection *)>;

  Connection(int fd, Epoll *ep);
  ~Connection();

  int fd() const { return fd_; }

  void onReadable(); // EPOLLIN
  void onWritable(); // EPOLLOUT
  void send(const std::string &data);
  void close();

  void setMessageCallback(MessageCallback cb) { msg_cb_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { close_cb_ = std::move(cb); }

private:
  void enableWriting();
  void disableWriting();

private:
  int fd_;
  Epoll *epoll_;  // 用于修改监听事件
  Buffer input_;  // 收到的数据
  Buffer output_; // 等待发送的数据

  MessageCallback msg_cb_;
  CloseCallback close_cb_;
};

} // namespace synthesizer::net
