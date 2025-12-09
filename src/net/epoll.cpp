#include <cstring>
#include <stdexcept>
#include <synthesizer/net/epoll.hpp>
#include <unistd.h>

namespace synthesizer::net {

Epoll::Epoll(int maxEvents) : epollFd_(-1), events_(maxEvents) {
  epollFd_ = ::epoll_create1(0);
  if (epollFd_ < 0) {
    throw std::runtime_error("epoll_create1 failed");
  }
}

Epoll::~Epoll() {
  if (epollFd_ >= 0) {
    ::close(epollFd_);
  }
}

bool Epoll::add(int fd, uint32_t events, void *userPtr) {
  epoll_event ev{};
  ev.events = events;
  ev.data.ptr = userPtr;
  return ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoll::mod(int fd, uint32_t events, void *userPtr) {
  epoll_event ev{};
  ev.events = events;
  ev.data.ptr = userPtr;
  return ::epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

// 无 ptr 的版本（方便 Acceptor 使用）
bool Epoll::add(int fd, uint32_t events) {
  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;
  return ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoll::mod(int fd, uint32_t events) {
  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;
  return ::epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoll::del(int fd) {
  return ::epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

int Epoll::wait(int timeout) {
  int nfds = ::epoll_wait(epollFd_, events_.data(), events_.size(), timeout);
  return nfds;
}

} // namespace synthesizer::net
