#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <assert.h>
#include <iostream>
#include <string>

#define DLOG(key) stream_wrapper{}
#define LOG(key) LOG_##key
#define LOG_INFO stream_wrapper{&std::cout}
#define LOG_ERROR stream_wrapper{&std::cerr}
#define LOG_FATAL stream_wrapper{&std::cerr}

class stream_wrapper {
public:
  explicit stream_wrapper(std::ostream* stream = nullptr)
    : stream_(stream) {}

  stream_wrapper(const stream_wrapper&) = delete;
  stream_wrapper& operator=(const stream_wrapper&) = delete;

  ~stream_wrapper() {
    if (stream_) {
      *stream_ << std::endl;
    }
  }

  template <class T>
  stream_wrapper& operator<<(const T& that) {
    if (stream_) {
      *stream_ << that;
    }
    return *this;
  }

private:
  std::ostream* stream_;
};

#define CHECK(expr) assert((expr))
#define CHECK_NOTNULL(expr) assert((expr) != nullptr)
#define CHECK_EQ(a, b) assert((a) == (b))
#define CHECK_NE(a, b) assert((a) != (b))
#define CHECK_LT(a, b) assert((a) < (b))
#define CHECK_GE(a, b) assert((a) >= (b))

namespace google {
  inline void InitGoogleLogging(const char*) {}
  inline void ShutdownGoogleLogging() {}
  inline void InstallFailureSignalHandler() {}
}

#endif
