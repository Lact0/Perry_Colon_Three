CC = g++
FIL = engine.o polyglotReader.o

main.exe: main.o ${FIL}
	${CC} -o main main.o ${FIL} ${GoodPractice}

main.o: src/main.cpp
	${CC} -c $<

%.o: src/%.cpp src/%.h
	${CC} -c $<

clean:
	del *.o