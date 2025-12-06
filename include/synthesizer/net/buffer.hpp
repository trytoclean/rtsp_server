#pragma once
#include <cstring>
#include <string>
#include <vector>

namespace synthesizer::net {

class Buffer {
public:
  static const size_t kInitialSize = 1024;

  Buffer(size_t initialSize = kInitialSize)
      : buffer_(initialSize), readerIndex_(0), writerIndex_(0) {}

  // 可读字节数
  size_t readableBytes() const { return writerIndex_ - readerIndex_; }
  // 可写字节数
  size_t writableBytes() const { return buffer_.size() - writerIndex_; }
  // 前置空间（用于腾挪）
  size_t prependableBytes() const { return readerIndex_; }

  // 返回读指针
  const char *readBegin() const { return buffer_.data() + readerIndex_; }
  // 返回写指针
  char *writeBegin() { return buffer_.data() + writerIndex_; }

  // 增加写入位置
  void hasWritten(size_t len) { writerIndex_ += len; }
  // 增加读取位置
  void hasRead(size_t len) { readerIndex_ += len; }

  // 将所有可读数据取出为 string（用于解析）
  std::string retrieveAllAsString() {
    std::string result(readBegin(), readableBytes());
    retrieveAll();
    return result;
  }

  // 清空
  void retrieveAll() { readerIndex_ = writerIndex_ = 0; }

  // 确保 writable 字节足够
  void ensureWritable(size_t len) {
    if (writableBytes() < len) {
      makeSpace(len);
    }
  }

  // append 数据（用于写入 output buffer）
  void append(const char *data, size_t len) {
    ensureWritable(len);
    std::memcpy(writeBegin(), data, len);
    writerIndex_ += len;
  }

  void append(const std::string &s) { append(s.data(), s.size()); }

private:
  // 扩容或腾挪空间
  void makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len) {
      // 扩容
      buffer_.resize(writerIndex_ + len);
    } else {
      // 腾挪
      size_t readable = readableBytes();
      std::memmove(buffer_.data(), readBegin(), readable);
      readerIndex_ = 0;
      writerIndex_ = readable;
    }
  }

private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};

} // namespace synthesizer::net
