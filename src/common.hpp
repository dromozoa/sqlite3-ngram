#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <assert.h>
#include <iostream>
#include <string>

#define LOG(name) LOG_IMPL(#name)
#define DLOG(name) LOG_IMPL(#name)
#define CHECK(name) LOG_IMPL(#name)

inline std::ostream& LOG_IMPL(const std::string& name) {
  if (name == "INFO" || name == "ok") {
    return std::cout;
  } else {
    return std::cerr;
  }
}

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

#define BUILD_HEAD_COMMIT "(BUILD_HEAD_COMMIT)"
#define BUILD_USER "(BUILD_USER)"
#define BUILD_TIMESTAMP "(BUILD_TIMESTAMP)"

#endif
