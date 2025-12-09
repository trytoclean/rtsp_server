#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <synthesizer/net/socket.hpp>
#include <sys/socket.h>
#include <unistd.h>

namespace synthesizer::net {

Socket::Socket() {
  fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw std::runtime_error("socket() failed");
  }
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::~Socket() {
  if (fd_ >= 0) {
    ::close(fd_);
  }
}

void Socket::bindSocket(const InetAddress &addr) {
  const sockaddr_in &raw = addr.addr_;

  if (::bind(fd_, (sockaddr *)&raw, addr.size()) < 0) {
    throw std::runtime_error("bind() failed");
  }
}

void Socket::listenSocket(int backlog) {
  if (::listen(fd_, backlog) < 0) {
    throw std::runtime_error("listen() failed");
  }
}

void Socket::setNonBlocking() { ::fcntl(fd_, F_SETFL, O_NONBLOCK); }

} // namespace synthesizer::net
