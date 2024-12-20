all:
	g++ -I SDL2/x86_64-w64-mingw32/include -L SDL2/x86_64-w64-mingw32/lib -o main main.cpp -l mingw32 -l SDL2main -l SDL2 --std=c++17 -Wall -Werror -pedantic -g
