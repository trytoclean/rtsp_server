#pragma once

#include <netinet/in.h>

namespace synthesizer::net {

class InetAddress {
public:
  InetAddress();
  InetAddress(const char *ip, int port);
  ~InetAddress();

  const sockaddr_in *getSockAddr() const { return &addr_; }
  socklen_t size() const { return sizeof(addr_); }
  sockaddr_in addr_{};
  socklen_t addr_size;
};

} // namespace synthesizer::net
