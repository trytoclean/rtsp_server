#include <RtspParser.h>
#include <algorithm>
#include <sstream>
#include <stdexcept>
static inline std::string trim(std::string s) {
  auto not_space = [](unsigned char c) { return !std::isspace(c); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
  s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
  return s;
}

static inline std::string tolower_str(std::string s) {
  for (char &c : s)
    c = std::tolower((unsigned char)c);
  return s;
}

RtspRequest RtspParser::parse(const std::string &data) {
  RtspRequest req;
  size_t pos = 0;

  // 1) request line
  size_t line_end = data.find("\r\n", pos);
  if (line_end == std::string::npos)
    throw std::runtime_error("no request line");
  std::string request_line = data.substr(pos, line_end - pos);
  request_line = trim(request_line);

  std::istringstream iss(request_line);
  std::string method_str, uri_str, ver_str;
  if (!(iss >> method_str >> uri_str >> ver_str))
    throw std::runtime_error("bad request line");

  req.method = stringToMethod(method_str);
  req.uri = uri_str;
  req.version = ver_str; // 可校验 RTSP/1.0

  // 2) headers
  pos = line_end + 2;
  while (true) {
    line_end = data.find("\r\n", pos);
    if (line_end == std::string::npos)
      break;
    if (line_end == pos) {
      pos += 2;
      break;
    } // \r\n\r\n end headers

    std::string header_line = data.substr(pos, line_end - pos);
    size_t colon = header_line.find(':');
    if (colon != std::string::npos) {
      std::string name = tolower_str(trim(header_line.substr(0, colon)));
      std::string value = trim(header_line.substr(colon + 1));
      req.headers[name] = value; // 若要支持重复 header，改为 vector
    }
    pos = line_end + 2;
  }

  // 3) body (Content-Length)
  auto it = req.headers.find("content-length");
  if (it != req.headers.end()) {
    int len = std::stoi(it->second);
    if (pos + len <= data.size())
      req.body = data.substr(pos, len);
  }

  return req;
}

Method RtspParser::stringToMethod(const std::string &method) {
  if (method == "OPTIONS")
    return Method::OPTIONS;
  if (method == "DESCRIBE")
    return Method::DESCRIBE;
  if (method == "SETUP")
    return Method::SETUP;
  if (method == "PLAY")
    return Method::PLAY;
  if (method == "PAUSE")
    return Method::PAUSE;
  if (method == "TEARDOWN")
    return Method::TEARDOWN;
  if (method == "ANNOUNCE")
    return Method::ANNOUNCE;
  if (method == "RECORD")
    return Method::RECORD;
  if (method == "GET_PARAMETER")
    return Method::GET_PARAMETER;
  if (method == "SET_PARAMETER")
    return Method::SET_PARAMETER;
  return Method::UNKNOWN;
}