UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
  # Mac
  CC := clang++ -arch x86_64
  LDFLAGS := 
else
  # Linux
  CC := g++
  LDFLAGS := -L/usr/local/lib 
endif

CPPFLAGS := -I/usr/local/include -I./ -I./entities
CFLAGS := -std=c++11 -O3 -Wall -Wno-unused-function

######################################

BIN := spsim
SRCS := $(wildcard *.cpp) $(wildcard entities/*.cpp)
INCLUDES := $(wildcard *.hpp) $(wildcard entities/*.hpp)

all: $(BIN)

$(BIN): $(SRCS) $(INCLUDES)
	$(CC) $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(CFLAGS)

clean:
	rm -f *~ *.o $(BIN)
