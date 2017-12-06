all:
	g++ -std=c++11 -O2 -Wall -Wextra -pedantic -o simulator simulator.cpp 

run:
	./simulator jaro.txt
