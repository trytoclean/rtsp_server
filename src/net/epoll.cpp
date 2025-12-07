#include <synthesizer/net/epoll.hpp>
#include <sys/epoll.h>

namespace synthesizer::net {

Epoll::Epoll(int maxEvent) {}

bool Epoll::add(int fd, uint32_t events) {
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.fd = fd; // server_fd
  return epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}
} // namespace synthesizer::net