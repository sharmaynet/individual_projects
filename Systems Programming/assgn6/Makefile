CFLAGS = -Wall -pedantic -g
LFLAGS = -Wall -g
MAIN = mush

all: $(MAIN)

$(MAIN): mush.o parseline.o
	gcc $(CFLAGS) -o $(MAIN) mush.o parseline.o

mush.o: mush.c parseline.h
	gcc $(CFLAGS) -c mush.c
parseline.o : parseline.c parseline.h
	gcc $(CFLAGS) -c parseline.c

clean :
	rm -f *.o
