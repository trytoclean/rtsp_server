#pragma once
#include <string>
#include <unordered_map>
enum class Method {
  OPTIONS,
  DESCRIBE,
  SETUP,
  PLAY,
  PAUSE,
  TEARDOWN,
  ANNOUNCE,
  RECORD,
  GET_PARAMETER,
  SET_PARAMETER,
  UNKNOWN
};

struct RtspRequest {
  Method method;       // enum class Method
  std::string uri;     // 原始URI
  std::string version; // RTSP/1.0
  std::unordered_map<std::string, std::string> headers;
  std::string body;

  // 解析出来的常用字段（可选缓存）
  int cseq = -1;
  std::string session_id;
  std::string transport;
};

class RtspParser {
public:
  RtspRequest parse(const std::string &raw);

private:
  Method stringToMethod(const std::string &method_str);
};
