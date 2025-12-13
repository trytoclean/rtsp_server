/**
 * @file epoll.hpp
 * @brief RTSP请求分发器：根据 Method 路由到对应 Handler
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
#include <cstdint>
#include <sys/epoll.h>
#include <vector>

namespace synthesizer::net {

class Epoll {
public:
  explicit Epoll(int maxEvents = 1024);
  ~Epoll();

  // 支持 data.ptr
  bool add(int fd, uint32_t events, void *userPtr);
  bool mod(int fd, uint32_t events, void *userPtr);

  // data.fd 版本（给 Acceptor 用）
  bool add(int fd, uint32_t events);
  bool mod(int fd, uint32_t events);

  bool del(int fd);

  int wait(int timeout = -1);

  const epoll_event &event(int i) const { return events_[i]; }
  const std::vector<epoll_event> &getEvents() const { return events_; }

private:
  int epollFd_;
  std::vector<epoll_event> events_;
};

} // namespace synthesizer::net
