CXXFLAGS = \
  -std=c++11 \
  -Wpedantic \
  -Wall \
  -Wextra \
  -Wconversion \
  -Wsign-conversion \
  $(INCLUDES) \
  $(OPTFLAGS)

INCLUDES = \
  -isystem ../../include

OPTFLAGS = \
  -O2


.PHONY: run clean
.SUFFIXES: .cc

run: main
	./main

clean:
	rm -f main
