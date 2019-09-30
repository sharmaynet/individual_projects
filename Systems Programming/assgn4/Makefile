CFLAGS = -Wall -pedantic -g
MAIN = mytar

LFLAGS = -Wall -g

all: $(MAIN)

$(MAIN): main.o mytar.o
	gcc $(LFLAGS) -o mytar main.o mytar.o

main.o : main.c mytar.h
	gcc $(CFLAGS) -c main.c

mytar.o : mytar.c mytar.h
	gcc $(CFLAGS) -c mytar.c


clean :
	rm -f *.o
