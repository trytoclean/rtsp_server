#pragma once
#include <RtspHandlers.h>
#include <RtspParser.h>
#include <memory>

class RtspDispatcher {
public:
  RtspDispatcher();
  RtspResponse dispatch(const RtspRequest &req);

private:
  std::unordered_map<Method, std::unique_ptr<IRtspHandler>> handlers_;
};