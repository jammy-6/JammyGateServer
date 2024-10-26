#ifndef BUFFER_H
#define BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <assert.h>
#include <atomic>
#include <cstring> //perror
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/uio.h> //readv
#include <unistd.h>  // write
#include <vector>    //readv

class Buffer {

private:
  char *BeginPtr_();
  const char *BeginPtr_() const;
  void MakeSpace_(size_t len);
  // buff实际储存的数据
  std::string buff_;
  std::atomic<std::size_t> readPos_;
  std::atomic<std::size_t> writePos_;

public:
  Buffer(int initBuffSize = 1024);
  ~Buffer() = default;

  size_t WritableBytes() const;
  size_t ReadableBytes() const;
  size_t PrependableBytes() const;

  const char *Peek() const;
  void EnsureWriteable(size_t len);
  void HasWritten(size_t len);

  void Retrieve(size_t len);
  void RetrieveUntil(const char *end);

  void RetrieveAll();
  std::string RetrieveAllToStr();

  const char *BeginWriteConst() const;
  char *BeginWrite();

  void Append(const std::string &str);
  void Append(const char *str, size_t len);
  void Append(const void *data, size_t len);
  void Append(const Buffer &buff);

  ssize_t ReadFd(int fd, int *Errno);
  ssize_t WriteFd(int fd, int *Errno);

  // std::mutex mut_;//用于保护缓冲区
  // 往buffer中添加数据
  // void append(const char *data, int size);
  // // 返回buff大小
  // int size();
  // // 返回首地址指针
  // const char *ptr();
  // // 情况当前缓存
  // void clear();
  // // 从buff_的头部取出size大小的数据
  // void pop(int size);
  // 从buff_中获取一条消息进行处理
  bool getMsg(std::string &msg);
  // 从buff_中获取一个http请求
  bool getHttpMsg(std::string &msg);

  std::string &getStr();
};

#endif