// include/rtsp_server/net/tcp_server.h
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "rtsp_server/net/connection.h"

namespace rtsp_server::net {

// 上层回调：收到数据时触发
using OnMessageCallback =
    std::function<void(Connection *conn, const std::string &data)>;

class TcpServer {
public:
  TcpServer(std::string bind_ip, uint16_t port);
  ~TcpServer();

  TcpServer(const TcpServer &) = delete;
  TcpServer &operator=(const TcpServer &) = delete;

  void setOnMessage(OnMessageCallback cb) { on_message_ = std::move(cb); }

  bool start(); // listen + loop
  void stop();

private:
  bool initSocket_();
  void loop_();               // accept/recv 主循环
  void handleClient_(int fd); // 单连接处理（现在先同步 MVP）

  std::string bind_ip_;
  uint16_t port_;
  int listen_fd_ = -1;
  bool running_ = false;

  OnMessageCallback on_message_;
};

} // namespace rtsp_server::net
