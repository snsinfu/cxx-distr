CXXFLAGS = \
  -std=c++11 \
  -Wpedantic \
  -Wall \
  -Wextra \
  -Wconversion \
  -Wsign-conversion \
  $(INCLUDES) \
  $(DBGFLAGS) \
  $(OPTFLAGS) \
  $(EXTRA_CXXFLAGS)

INCLUDES = \
  -isystem include \
  -I ../include

DBGFLAGS = \
  -g \
  -fsanitize=address \
  -DDISTR_DEBUG

OPTFLAGS = \
  -Og

ARTIFACTS = \
  main \
  $(OBJECTS)

OBJECTS = \
  main.o \
  test_discrete_distribution.o \
  test_discrete_weights.o

DEPENDS = \
  ../include/discrete_distribution.hpp


.PHONY: run clean
.SUFFIXES: .cc

run: main
	./main

clean:
	rm -f $(ARTIFACTS)

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

.cc.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_discrete_distribution.o: test_discrete_distribution.cc $(DEPENDS)
test_discrete_weights.o: test_discrete_weights.cc $(DEPENDS)
