#pragma once
#include <synthesizer/rtsp/rtsp_response.h>
class IRtspHandler {
public:
  virtual ~IRtspHandler() = default;
  virtual RtspResponse handle(const RtspRequest &req) = 0;
};

class OptionsHandler : public IRtspHandler {
public:
  RtspResponse handle(const RtspRequest &req) override;
};

// class DescribeHandler : public IRtspHandler {
// public:
//   RtspResponse handle(const RtspRequest &req) override {
//     auto resp = RtspResponseBuilder::ok(req);
//     resp.headers["Content-Type"] = "application/sdp";
//     resp.body = sdp_for(req.uri); // 你现在可以先写死
//     resp.headers["Content-Length"] = std::to_string(resp.body.size());
//     return resp;
//   }
// };