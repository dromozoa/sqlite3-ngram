#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <assert.h>
#include <iostream>
#include <string>

#define LOG_IMPL(name) stream_wrapper{&std::cout}
#define LOG(name) LOG_IMPL(name)
#define DLOG(name) LOG_IMPL(name)
#define CHECK(name) LOG_IMPL(name)

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

#define CHECK_NOTNULL(expr) assert((expr) != nullptr)
#define CHECK_EQ(a, b) assert((a) == (b))
#define CHECK_NE(a, b) assert((a) != (b))
#define CHECK_GE(a, b) assert((a) >= (b))
#define CHECK_LT(a, b) assert((a) < (b))

namespace google {
  inline void InitGoogleLogging(const char*) {}
  inline void ShutdownGoogleLogging() {}
  inline void InstallFailureSignalHandler() {}
}

#endif
