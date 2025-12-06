#pragma once
#include <sys/epoll.h>
#include <vector>

namespace synthesizer::net {

class Epoll {
public:
  Epoll(int maxEvents = 1024);
  ~Epoll();

  bool add(int fd, uint32_t events);
  bool modify(int fd, uint32_t events);
  bool remove(int fd);

  int wait(int timeout = -1);

  const std::vector<epoll_event> &getEvents() const { return events_; }

private:
  int epollFd_;
  std::vector<epoll_event> events_;
};

} // namespace synthesizer::net
