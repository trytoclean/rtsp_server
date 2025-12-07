#pragma once
#include "inet_address.hpp"
#include <sys/socket.h>
namespace synthesizer::net {

class Socket {
public:
  explicit Socket();
  ~Socket();
  void bindSocket(InetAddress *addr_);
  void listenSocket(int listen_queue_len = 10);

private:
  InetAddress addr_;
  int fd;
};

} // namespace synthesizer::net