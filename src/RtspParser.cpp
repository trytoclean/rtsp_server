#include <RtspParser.h>

#include <stdexcept>
RtspRequest RtspParser::parse(const std::string &data) {
  RtspRequest req;
  size_t pos = 0;
  size_t line_end = data.find("\r\n", pos);
  if (line_end == std::string::npos) {
    throw std::runtime_error("Invalid RTSP request: no request line");
  }

  // Parse request line
  std::string request_line = data.substr(pos, line_end - pos);
  size_t method_end = request_line.find(' ');
  if (method_end == std::string::npos) {
    throw std::runtime_error("Invalid RTSP request: no method");
  }
  std::string method_str = request_line.substr(0, method_end);
  req.method = stringToMethod(method_str);

  size_t uri_end = request_line.find(' ', method_end + 1);
  if (uri_end == std::string::npos) {
    throw std::runtime_error("Invalid RTSP request: no URI");
  }
  req.uri = request_line.substr(method_end + 1, uri_end - method_end - 1);

  req.version = request_line.substr(uri_end + 1);

  // Parse headers
  pos = line_end + 2;
  while (true) {
    line_end = data.find("\r\n", pos);
    if (line_end == std::string::npos || line_end == pos) {
      break; // End of headers
    }
    std::string header_line = data.substr(pos, line_end - pos);
    size_t colon_pos = header_line.find(':');
    if (colon_pos != std::string::npos) {
      std::string header_name =
          header_line.substr(0, colon_pos); // Trim spaces if needed
      std::string header_value =
          header_line.substr(colon_pos + 1); // Trim spaces if needed
      req.headers[header_name] = header_value;
    }
    pos = line_end + 2;
  }

  // Body parsing can be added here if needed

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