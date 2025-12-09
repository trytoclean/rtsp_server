#pragma once

#include <netinet/in.h>

namespace synthesizer::net {

class InetAddress {
public:
  InetAddress();
  InetAddress(const char *ip, int port);
  InetAddress(int port);

  ~InetAddress();
  socklen_t size() const { return sizeof(addr_); }
  sockaddr_in addr_{};
};

} // namespace synthesizer::net
