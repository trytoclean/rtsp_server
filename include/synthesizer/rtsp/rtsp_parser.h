#pragma once
#include <string>
#include <unordered_map>

namespace synthesizer::rtsp {
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
  Method method;
  std::string uri;
  std::string version;
  std::unordered_map<std::string, std::string> headers;
  std::string body;

  int cseq = -1;
  std::string session_id;
  std::string transport;
};

class RtspParser {
public:
  RtspRequest parse(const std::string &raw);

private:
  Method stringToMethod(const std::string &method_str);
  std::string methodToString(Method method);
};
} // namespace synthesizer::rtsp