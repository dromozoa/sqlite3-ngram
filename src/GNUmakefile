HEAD_COMMIT = $()

CPPFLAGS = -DDROMOZOA_NO_HIGHRIGHT=1 -DDROMOZOA_NO_GOOGLE_LOGGING=1
CXXFLAGS = -Wall -W -std=c++11 -g -O2 -fPIC
LDFLAGS = -dynamiclib
LDLIBS = -lsqlite3 -ldl

OBJS = ngram.o utils.o token_vector.o
TARGET = libngram.so

# -bundle -undefined dynamic_lookup -all_load
# -lsqlite3 -lpthread -ldl -o sqlite3.so

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBFLAG) $^ $(LDLIBS) -o $@

clean::
	$(RM) $(TARGET) $(OBJS)

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<
