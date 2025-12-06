#pragma once
#include "buffer.hpp"
#include <functional>
#include <memory>
namespace synthesizer::net {

class Epoll;

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(int fd, Epoll *loop);
  ~Connection();

  void onReadable();
  void onWritable();
  void close();

  // 业务层调用，用于发送响应
  void send(const std::string &data);

  // 设置回调（Parser / Dispatcher）
  void setMessageCallback(std::function<void(Connection *, Buffer &)> cb);
  void setCloseCallback(std::function<void(Connection *)> cb);

private:
  int fd;
  bool closed{false};

  Buffer input;
  Buffer output;

  Epoll *loop;

  std::function<void(Connection *, Buffer &)> messageCallback;
  std::function<void(Connection *)> closeCallback;

  void handleError();
};

} // namespace synthesizer::net
