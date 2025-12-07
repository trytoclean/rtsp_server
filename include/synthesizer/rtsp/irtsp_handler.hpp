#pragma once
#include "rtsp_response_builder.hpp"
namespace synthesizer::rtsp {
class IRtspHandler {
public:
  virtual ~IRtspHandler() = default;

  virtual RtspResponse handle(const RtspRequest &req) = 0;
};
} // namespace synthesizer::rtsp