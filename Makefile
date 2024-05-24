

GoodPractice = -pedantic-errors

all:
	g++ src/main.cpp src/polyglotReader.cpp -o main ${GoodPractice}