CFLAGS = -Wall -ansi -pedantic -g
MAIN = htable
MAIN2 = hencode
MAIN3 = hdecode
LFLAGS = -Wall -g

all: $(MAIN) $(MAIN2) $(MAIN3)

$(MAIN): htable.o bst.o
	gcc $(LFLAGS) -o htable htable.o bst.o

$(MAIN2): hencode.o bst.o
	gcc $(LFLAGS) -o hencode hencode.o bst.o

$(MAIN3): hdecode.o  bst.o
	gcc $(LFLAGS) -o hdecode hdecode.o bst.o

htable.o : htable.c bst.h
	gcc $(CFLAGS) -c htable.c

hencode.o : hencode.c bst.h
	gcc $(CFLAGS) -c hencode.c

hdecode.o: hdecode.c bst.h
	gcc $(CFLAGS) -c hdecode.c

bst.o : bst.c bst.h
	gcc $(CFLAGS) -c bst.c

test: htable
	./htable test2.txt > t1.txt
	~pn-cs357/demos/htable test2.txt > t2.txt
	diff t1.txt t2.txt
	./htable dec_of_ind.txt > t1.txt
	~pn-cs357/demos/htable dec_of_ind.txt > t2.txt
	diff t1.txt t2.txt
	./htable onechar2.txt > t1.txt
	~pn-cs357/demos/htable onechar2.txt > t2.txt
	diff t1.txt t2.txt
	./htable alphabet.txt > t1.txt
	~pn-cs357/demos/htable alphabet.txt > t2.txt
	diff t1.txt t2.txt

	./hencode test2.txt > t1.huff
	~pn-cs357/demos/hencode test2.txt > t2.huff
	diff t1.huff t2.huff
	./hencode dec_of_ind.txt > a1.huff
	~pn-cs357/demos/hencode dec_of_ind.txt > a2.huff
	diff a1.huff a2.huff
	./hencode onechar2.txt > b1.huff
	~pn-cs357/demos/hencode onechar2.txt > b2.huff
	diff b1.huff b2.huff
	./hencode alphabet.txt > c1.huff
	~pn-cs357/demos/hencode alphabet.txt > c2.huff
	diff c1.huff c2.huff

	./hdecode t1.huff > t1.o
	~pn-cs357/demos/hdecode t2.huff > t2.o
	diff t1.o t2.o
	./hdecode a1.huff > a1.o
	~pn-cs357/demos/hdecode a2.huff > a2.o
	diff a1.o a2.o
	./hdecode b1.huff > b1.o
	~pn-cs357/demos/hdecode b2.huff > b2.o
	diff b1.o b2.o
	./hdecode c1.huff > c1.o
	~pn-cs357/demos/hdecode c2.huff > c2.o
	diff c1.o c2.o

clean :
	rm -f *.o
	rm -f *.huff
