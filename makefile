CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

OBJS = main.o utils.o parsing_input.o generated_words.o load_data.o render.o gameloop.o leaderboard.o

all: game

game: $(OBJS)
	$(CC) $(CFLAGS) -o game $(OBJS)

main.o: main.c data.h gameloop.h leaderboard.h utils.h
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

parsing_input.o: parsing_input.h utils.h
	$(CC) $(CFLAGS) -c parsing_input.c

generated_words.o: generated_words.h
	$(CC) $(CFLAGS) -c generated_words.c

load_data.o: load_data.h
	$(CC) $(CFLAGS) -c load_data.c

render.o: render.h utils.h
	$(CC) $(CFLAGS) -c render.c

gameloop.o: gameloop.h leaderboard.h parsing_input.h load_data.h utils.h render.h 
	$(CC) $(CFLAGS) -c gameloop.c

leaderboard.o: leaderboard.h
	$(CC) $(CFLAGS) -c leaderboard.c

clean:
	rm -f *.o game
