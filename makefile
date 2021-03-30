parell: main.o lista.o
	gcc -o parell main.o lista.o -lpthread -fopenmp

main.o: main.c lista.h
	gcc -c main.c
	
lista.o: lista.c lista.h
	gcc -c lista.c	
	
