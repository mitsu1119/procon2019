
.PHONY: all
all: ../include/useful.hpp ../src/useful.cpp ./param.cpp
	g++ ../src/useful.cpp param.cpp -o ../build/src/calc -I ../include

.PHONY: tester
tester: test.cpp
	g++ -o tester test.cpp
