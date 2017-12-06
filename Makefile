all:
	g++ -std=c++11 -O2 -Wall -Wextra -pedantic -o generate_kwh generate_kwh.cpp
	g++ -std=c++11 -O2 -Wall -Wextra -pedantic -o simulator simulator.cpp 
