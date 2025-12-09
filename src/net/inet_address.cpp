#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>

namespace synthesizer::net {

class InetAddress {
public:
  InetAddress() = default;

  InetAddress(const char *ip, int port) {
    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr_.sin_addr);
  }

  explicit InetAddress(int port) { // 用于绑定 0.0.0.0
    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = htons(port);
  }

  const sockaddr_in &get() const { return addr_; }
  socklen_t size() const { return sizeof(addr_); }

private:
  sockaddr_in addr_{};
};

} // namespace synthesizer::net
