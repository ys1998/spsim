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

CPPFLAGS := -I/usr/local/include -I./ -I./entities/includes
CFLAGS := -std=c++17 -Wall #-O3

######################################

BIN := spsim
SRCS := $(wildcard *.cpp) $(wildcard *.hpp) $(wildcard entities/src/*.cpp) $(wildcard entities/includes/*.hpp)
INCLUDES := $(wildcard *.hpp) $(wildcard entities/includes/*.hpp)

all: $(BIN)

$(BIN): $(SRCS) $(INCLUDES)
	$(CC) $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(CFLAGS)

clean:
	rm -f *~ *.o $(BIN)
