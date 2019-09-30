CFLAGS = -Wall -pedantic -g
LFLAGS = -Wall -g
MAIN = parseline 

all: $(MAIN)

$(MAIN): parseline.o 
	gcc $(CFLAGS) -o $(MAIN) parseline.o 

parseline.o : parseline.c parseline.h
	gcc $(CFLAGS) -c parseline.c
	
clean :
	rm -f *.o
