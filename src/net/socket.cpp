#include <cstdio>
#include <synthesizer/net/inet_address.hpp>
#include <synthesizer/net/socket.hpp>
#include <sys/socket.h>

namespace synthesizer::net {

Socket::Socket() {

  fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("can't make socket");
  }
};
void Socket::bindSocket(InetAddress *address) {
  if (::bind(fd, reinterpret_cast<sockaddr *>(&address->addr_),
             address->addr_size) < 0) {
    perror("bind fault");
  }
}

Socket::~Socket() = default;
void Socket::listenSocket(int listen_queue_len) {
  if (::listen(fd, listen_queue_len) < 0) {
    perror("listen error");
  }
}
} // namespace synthesizer::net