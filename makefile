parell: main.o threads.o lista.o
	gcc -o parell main.o threads.o lista.o -lpthread -fopenmp

main.o: main.c threads.c lista.h
	gcc -c main.c
	
lista.o: lista.c lista.h
	gcc -c lista.c	

threads.o: threads.c threads.h lista.h -lpthread
	gcc -c threads.c