/**
 * @file inet_address.hpp
 * @brief 分配ip地址,支持INADDR_ANY
 *
 * 典型调用：
 * InetAddress("127.0.0.1", 8080)
 *
 * @author trytoclean
 * @date 2025-11-23
 */
#pragma once
#include <netinet/in.h>
namespace synthesizer::net {

class InetAddress {
public:
  InetAddress();
  InetAddress(const char *ip, int port);
  // 用于绑定 0.0.0.0
  InetAddress(int port);

  ~InetAddress();
  socklen_t size() const { return sizeof(addr_); }
  sockaddr_in addr_{}; //{}初始化
};

} // namespace synthesizer::net
