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
