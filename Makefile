CC = g++
SRCS = $(wildcard src/*.cpp)
DOTO = $(SRCS:src/%.cpp=%.o)
FLG =

all: main.exe

debug: FLG += -g -ggdb
debug: clean main.exe reclean

release: FLG += -O2 -DNDEBUG 
release: clean main.exe reclean

main.exe: ${DOTO}
	${CC} -o main ${DOTO} ${FLG} 

main.o: src/main.cpp
	${CC} -c $< ${FLG}

%.o: src/%.cpp src/%.h
	${CC} -c $< ${FLG}

clean:
	touch temp.o
	rm *.o
reclean:
	rm *.o