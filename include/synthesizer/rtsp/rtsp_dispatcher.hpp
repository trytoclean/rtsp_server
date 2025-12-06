#pragma once
#include <synthesizer/rtsp/handlers/describe_handler.hpp>
#include <synthesizer/rtsp/handlers/options_handler.hpp>
#include <synthesizer/rtsp/handlers/pause_handler.hpp>
#include <synthesizer/rtsp/handlers/play_handler.hpp>
#include <synthesizer/rtsp/handlers/setup_handler.hpp>
#include <synthesizer/rtsp/handlers/teardown_handler.hpp>
#include <synthesizer/rtsp/rtsp_parser.hpp>

namespace synthesizer::rtsp {
class Dispatcher {
public:
  RtspResponse disaptcher(const RtspRequest &request);
};
} // namespace synthesizer::rtsp