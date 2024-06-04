CC = g++
SRCS = $(wildcard src/*.cpp)
DOTO = $(SRCS:src/%.cpp=%.o)
DBG = #-g -ggdb

main.exe: ${DOTO}
	${CC} -o main ${DOTO} ${DBG} 

main.o: src/main.cpp
	${CC} -c $< ${DBG} 

%.o: src/%.cpp src/%.h
	${CC} -c $< ${DBG} 

clean:
	rm *.o