CC = g++
SRCS = $(wildcard src/*.cpp)
DOTO = $(SRCS:src/%.cpp=%.o)
DBG = #-g -ggdb

main.exe: ${DOTO}
	${CC} -o main ${DOTO} ${DBG} -pthread

main.o: src/main.cpp
	${CC} -c $< ${DBG} -pthread

%.o: src/%.cpp src/%.h
	${CC} -c $< ${DBG} -pthread

clean:
	rm *.o