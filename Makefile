all:
	g++ -I src/include -L src/lib ó main main.cpp -lmingw32 -lSDL2main -lSDL2  