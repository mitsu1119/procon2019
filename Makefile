.PHONY: all
all: run

run: ./src/main.o ./src/field.o ./src/agent.o
	g++ -o run ./src/main.o ./src/field.o ./src/agent.o

main.o: ./src/main.cpp
	g++ -c ./src/main.cpp

main.o: ./src/include/field.hpp

field.o: ./src/field.cpp
	g++ -c ./src/field.cpp

field.o: ./src/include/field.hpp ./src/include/agent.hpp

agent.o: ./src/agent.cpp
	g++ -c ./src/agent.cpp

agent.o: ./src/include/agent.hpp ./src/include/field.hpp


.PHONY: clean
clean:
	rm ./src/*.o
