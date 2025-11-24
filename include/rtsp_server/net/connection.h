// include/rtsp_server/net/connection.h
#pragma once
#include <string>

namespace rtsp_server::net {

class Connection {
public:
  explicit Connection(int fd);
  ~Connection();

  Connection(const Connection &) = delete;
  Connection &operator=(const Connection &) = delete;

  int fd() const { return fd_; }

  // 发送原始字节
  bool send(const std::string &data);

  // 关闭连接
  void close();

private:
  int fd_;
  bool closed_ = false;
};

} // namespace rtsp_server::net
