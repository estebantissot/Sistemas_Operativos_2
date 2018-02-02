DEPEND = funciones.h
CFLAGS = -Wall -pedantic -Werror

#Objetivo:
Objeto: servidor.o
	@gcc -o servidor sock_srv_i_cc.o

#Compilacion:
servidor.o: sock_srv_i_cc.c $(DEPEND)
	@gcc $(CFLAGS) -c sock_srv_i_cc.c


#Funciones:
clean:
	@rm sock_srv_i_cc.o
	@rm servidor
