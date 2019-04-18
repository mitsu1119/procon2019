.PHONY: all
all: run

run: ./src/main.o ./src/field.o
	g++ -o run ./src/main.o ./src/field.o

main.o: ./src/main.cpp
	g++ -c ./src/main.cpp

main.o: ./src/field.hpp

field.o: ./src/field.cpp
	g++ -c ./src/field.cpp

field.o: ./src/field.hpp

.PHONY: clean
clean:
	rm ./obj/*.o
