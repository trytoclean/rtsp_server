#include <sstream>
#include <string>
#include <synthesizer/rtsp/rtsp_parser.hpp>
namespace synthesizer::rtsp {

static std::string trim(const std::string &s) {
  auto start = s.find_first_not_of(" \t\r\n");
  auto end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  return s.substr(start, end - start + 1);
}

Method string_to_method(const std::string &s) {
  if (s == "OPTIONS")
    return Method::OPTIONS;
  if (s == "DESCRIBE")
    return Method::DESCRIBE;
  if (s == "SETUP")
    return Method::SETUP;
  if (s == "PLAY")
    return Method::PLAY;
  if (s == "PAUSE")
    return Method::PAUSE;
  if (s == "TEARDOWN")
    return Method::TEARDOWN;
  if (s == "ANNOUNCE")
    return Method::ANNOUNCE;
  if (s == "RECORD")
    return Method::RECORD;
  if (s == "GET_PARAMETER")
    return Method::GET_PARAMETER;
  if (s == "SET_PARAMETER")
    return Method::SET_PARAMETER;
  return Method::UNKNOWN;
}

RtspRequest RtspParser::parse(const std::string &raw) {
  RtspRequest req;

  // Step 1. 按行拆分
  std::istringstream ss(raw);
  std::string line;

  // Step 2. 解析“请求行”
  if (std::getline(ss, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();

    std::istringstream ls(line);
    std::string method_str;
    ls >> method_str >> req.uri >> req.version;

    req.method = string_to_method(method_str);
  }

  // Step 3. 解析Headers（直到遇到空行）
  while (std::getline(ss, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    if (line.empty())
      break; // header结束

    auto pos = line.find(':');
    if (pos == std::string::npos)
      continue;

    std::string key = trim(line.substr(0, pos));
    std::string val = trim(line.substr(pos + 1));
    req.headers[key] = val;
  }

  return req;
}

} // namespace synthesizer::rtsp