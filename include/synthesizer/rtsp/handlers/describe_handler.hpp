#include "../irtsp_handler.hpp"

namespace synthesizer::rtsp {

class DescribeHandler : public IRtspHandler {

  RtspResponse handle(const RtspRequest &req) override;
};
} // namespace synthesizer::rtsp