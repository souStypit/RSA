CC = gcc
CFLAGS = -Werror -Wextra -Wall
CRUNFILE = ./app

all: clean
	$(CC) $(CFLAGS) main.c -o $(CRUNFILE) -lgmp -lm

run:
	$(CRUNFILE)

install:
	sudo apt-get install libgmp3-dev

clean:
	rm -rf $(CRUNFILE)