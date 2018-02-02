#DEPEND =
CFLAGS = -Werror -Wall -pedantic


#OBJETIVO
Objetivo: cliente.o
	@gcc -o cliente sock_cli_i_cc.o


#COMPILACION:
cliente.o: sock_cli_i_cc.c # $(DEPEND)
	@gcc $(CFLAGS) -c sock_cli_i_cc.c

#FUNCIONES
clean:
	@rm sock_cli_i_cc.o 
	@rm cliente
