CFLAGS = -Wall -ansi -pedantic -g
LFLAGS = -Wall -g
MAIN = fw

all: $(MAIN)

$(MAIN): fw.o read_word.o hash_word.o
	gcc $(CFLAGS) -o fw fw.o read_word.o hash_word.o

fw.o : fw.c
	gcc $(CFLAGS) -c fw.c

read_word.o : read_word.c
	gcc $(CFLAGS) -c read_word.c

hash_word.o: hash_word.c
	gcc $(CFLAGS) -c hash_word.c

test:
	./fw -n 2 testfile.txt
	echo done

clean :
	rm -f *.o
