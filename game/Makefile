# compiler options
CC = g++
CFLAGS = -std=c++11 -g 
LDFLAGS += -lm -lglut -lGL

SRC = game.cpp

all: game

game: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o game $(LDFLAGS)

clean:
	rm -f game *.o