

GoodPractice = -pedantic-errors

all:
	g++ src/main.cpp src/polyglotReader.cpp src/engine.cpp -o main ${GoodPractice}