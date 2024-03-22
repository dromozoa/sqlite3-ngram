BUILD_HEAD_COMMIT = $(shell git describe --dirty --always --abbrev=7)
BUILD_TIMESTAMP = $(shell date "+%Y/%m/%d %H:%M:%S %z")
BUILD_USER = $(shell whoami)

OS=$(shell uname)
LDFLAGS_Linux = -shared
LDFLAGS_Darwin = -dynamiclib

CPPFLAGS += \
	-DDROMOZOA_NO_HIGHRIGHT=1 \
	-DDROMOZOA_NO_GOOGLE_LOGGING=1 \
	-DBUILD_HEAD_COMMIT='"$(BUILD_HEAD_COMMIT)"' \
	-DBUILD_TIMESTAMP='"$(BUILD_TIMESTAMP)"' \
	-DBUILD_USER='"$(BUILD_USER)"'
CXXFLAGS += -Wall -W -std=c++11 -g -O2 -fPIC
ifeq ($(shell uname),Darwin)
LDFLAGS += -dynamiclib
else
LDFLAGS += -shared
endif
LDLIBS += -lsqlite3 -ldl

OBJS = ngram.o utils.o token_vector.o
TARGET = libngram.so

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBFLAG) $^ $(LDLIBS) -o $@

clean::
	$(RM) $(TARGET) $(OBJS)

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<
