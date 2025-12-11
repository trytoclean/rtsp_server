#pragma once
#include <string>
#include <unordered_map>

namespace synthesizer::media {

class MediaSourceManager {
public:
  explicit MediaSourceManager(std::string root);

  // 返回真实文件路径
  std::string resolve(const std::string &uri);

  // 验证资源是否存在
  bool exists(const std::string &uri);

private:
  std::string root_;

  // 可做缓存（uri → real_path）
  std::unordered_map<std::string, std::string> mapCache_;
};

} // namespace synthesizer::media
