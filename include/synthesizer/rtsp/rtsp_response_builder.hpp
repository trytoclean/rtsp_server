#pragma once
#include <chrono>
#include <sstream>
#include <string>
#include <synthesizer/rtsp/rtsp_parser.hpp>
#include <unordered_map>
namespace synthesizer::rtsp {
struct RtspResponse {
  std::string version = "RTSP/1.0";
  int code = 200;
  std::string reason = "OK";
  std::unordered_map<std::string, std::string> headers;
  std::string body;
  std::string serialize();
};

class RtspResponseBuilder {
public:
  static RtspResponse buildSuccess(const RtspRequest &req);
  static RtspResponse buildError(const RtspRequest &req, int code,
                                 std::string reason);

private:
  RtspRequest rtsp_;
};

inline std::string get_time() {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&t);
  std::ostringstream oss;
  // Date: Tue, Sep 16 2025 09:05:36 GMT\r\n
  oss << "Date: " << tm.tm_mon << tm.tm_mday << tm.tm_year << tm.tm_hour << ":"
      << tm.tm_min << ":" << tm.tm_sec << "GMT";

  return oss.str();
}
} // namespace synthesizer::rtsp