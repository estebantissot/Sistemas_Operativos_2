#DEPEND =
CFLAGS = -fopenmp -Wall -pedantic 


#OBJETIVO
Objetivo: radar.o
	@gcc $(CFLAGS) -o radar radar.o -lm


#COMPILACION:
radar.o: radar.c # $(DEPEND)
	@gcc $(CFLAGS) -c radar.c -lm

#FUNCIONES
clean:
	@rm radar.o 
	@rm radar
