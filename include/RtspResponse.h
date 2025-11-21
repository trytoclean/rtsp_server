#pragma once
#include <RtspParser.h>
#include <string>
#include <unordered_map>
struct RtspResponse {
  int code = 200;
  std::string reason = "OK";
  std::unordered_map<std::string, std::string> headers;
  std::string body;

  std::string serialize() const; // 统一输出字符串
};

class RtspResponseBuilder {
public:
  static RtspResponse ok(const RtspRequest &req);
  static RtspResponse error(const RtspRequest &req, int code,
                            std::string reason);
};