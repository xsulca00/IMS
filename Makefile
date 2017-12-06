all:
	g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o generate_kwh generate_kwh.cpp
	g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o simulator simulator.cpp 
