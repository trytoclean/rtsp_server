#include <arpa/inet.h>
#include <synthesizer/net/inet_address.hpp>
#include <sys/socket.h>

namespace synthesizer::net {
InetAddress::InetAddress(const char *ip, int port) {
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(ip);
  addr_.sin_port = htons(port);
}
InetAddress::~InetAddress() = default;
} // namespace synthesizer::net