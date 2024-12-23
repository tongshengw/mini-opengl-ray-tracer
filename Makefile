all:
	g++ -ISDL2/x86_64-w64-mingw32/include -Iglad/include -LSDL2/x86_64-w64-mingw32/lib -o main main.cpp ./glad/src/glad.c -lmingw32 -lSDL2main -lSDL2 --std=c++17 -Wall -Werror -pedantic -g
