rede: main.o lista.o
	gcc -o rede main.o lista.o -lpthread

main.o: main.c lista.h
	gcc -c main.c
	
lista.o: lista.c lista.h
	gcc -c lista.c	
	
