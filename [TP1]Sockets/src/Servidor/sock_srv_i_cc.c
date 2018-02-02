/*
 *Programa: Servidor
 *Familia: AF_INET -> Conexion Internet
 *Tipo: SOCK_STREAM -> Orientado a Conexion FULL DUPLEX con control de errores 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include "funciones.h"

#define TAM 1400

void socket_read(int newsockfd, char buffer[TAM]);	/* Funcion para leer el socket */
void socket_write(int newsockfd, char mensaje[TAM]);	/* Funcion para escribir el socket */


int main( int argc, char *argv[] ) {
	int sockfd, newsockfd, puerto, pid;
	socklen_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	int login = 0;
	char comand[7];
	char usuario[20];
	char ip[20];
	char port[4];
	int user;
	int pass;
	char comando[24];
	char parametro[20];
	char lista[TAM-1];
	char lista_comandos[TAM-1];
	char acumulado_diario[TAM-1];
	char precipitacion_mensual[TAM-1];
	char lista_promedio[TAM-1];
	int n_estacion;


	sockfd = socket( AF_INET, SOCK_STREAM, 0);	/* AF_INET es para usar los protocolos ARPA de Internet */
												/* SOCK_STREAM es para usar la conexion TCP */
	if ( sockfd < 0 ) { /* Control de error */
		perror( " apertura de socket ");
		exit( 1 );
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	puerto = 6020;	/* Se define el numero de puerto */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	/* INADDR_ANY para selecionar automáticamente la dirección IP de la máquina sobre la que está ejecutando el proceso */
	serv_addr.sin_port = htons( puerto );	/* Guarda el puerto en la estructura del socket */

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){	/* Se asocia un socket con un puerto  */
		perror( "ligadura" );
		exit( 1 );
	}

    printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen(sockfd, 5);	/*  Queda escuchando el puerto hasta que haya un conexion entrante */
	clilen = sizeof(cli_addr);

	while( 1 ) {
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen ); /* Se acepta la conexion entrante creando un nuevo socket */
		if ( newsockfd < 0 ){	/* Control de error */
			perror( "accept" );
			exit( 1 );
		}

		pid = fork(); 
		if ( pid < 0 ) {
			perror( "fork" );
			exit( 1 );
		}

		if ( pid == 0 ) {  /* Proceso hijo */
			close( sockfd );

			while ( 1 ) {

				while(!login){

					socket_read(newsockfd,buffer);	/* Se lee el socket */

					if( !strcmp( "fin", buffer)){	/* Se verifica si el usuario termino la sesion */
						if (!login){
							printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
							exit(0);
						}
					}

					if ((strstr(buffer," ") != NULL) && (strstr(buffer,"@") != NULL) && (strstr(buffer,":") != NULL)){	/* Primer control de intento de conexion */
						strcpy(comand,"");
						strcpy(usuario,"");
						strcpy(ip,"22");
						strcpy(port,"");
						
						sscanf(buffer, "%[a-z] %[^@]@%[:$]:%[^\n]",comand,usuario,ip,port);	/* Se parsea el buffer */

						if (!strncmp("connect", comand, 7))	/* Se verifica que el comando sea connect*/ 
						{
							user = 0;
							pass = 0;

							if (!strcmp("user1", usuario))			/* Se Verifica el usuario */
								user = 1;

							socket_write(newsockfd, "Password");	/* Se pide Password */
					
							socket_read(newsockfd,buffer);			/* Se lee el socket */
					
							buffer[strlen(buffer)-1] = '\0';
							printf("%s\n", buffer );
							if (!strcmp("1", buffer))  				/* Se verifica la Password */
								pass = 1;
							
							printf("user: %d pass: %d\n", user, pass);
							if (user & pass){						/* Si los datos fueron correctos se notifica el inicio de sesion correcto */
								login = 1;
								socket_write(newsockfd, "login successful");
							}	

							else									/* Si los datos fueron incorrectos se notifica del error */
								socket_write( newsockfd, "nombre de usuario y/o contraseña incorrecto");
						}
					}
				}

				strcpy(comando,"");											/* Se limpia la variable "comando" y "parametro" */
				strcpy(parametro,"");	

				socket_read(newsockfd,buffer);				/* Se lee el socket y se guardan los datos en el buffer */
				sscanf(buffer,"%s %s",comando, parametro);	/* Se parsea el buffer */
				printf("comando: %s\n",comando );
				printf("parametro: %s\n", parametro);

				if (!strncmp("desconectar", comando, 11))					/* Se verifica si hay que "desconectar" */
					login = 0;	
 			
 				else if(!strncmp("help", comando, 4)){
 					strcpy(lista_comandos,"");
 					listar_comandos(lista_comandos);
 					socket_write(newsockfd,lista_comandos);
 				}

 				else if(!strncmp("listar", comando, 6)){					/* Se verifica si hay que "listar" */
 					strcpy(lista,"");	
					listar(lista);
 					socket_write(newsockfd,lista);
 				} 

 				else if (!strncmp("diario_precipitacion", comando, 20)){	/* Se verifica si hay que mostrar el acumulado diario de presipitacion */
 					strcpy(acumulado_diario,"");	
					n_estacion = atoi(parametro);
					diario_precipitacion(n_estacion, acumulado_diario);
					socket_write(newsockfd,acumulado_diario);
 				} 

 				else if (!strncmp("mensual_precipitacion", comando, 21)){	/* Se verifica si hay que mostrar el acumuludado mensual de presipitacion */
 					strcpy(precipitacion_mensual,"");	
 					n_estacion = atoi(parametro);
					mensual_precipitacion(n_estacion, precipitacion_mensual);
					socket_write(newsockfd,precipitacion_mensual);
 				} 

 				else if (!strncmp( "promedio", comando, 8)){				/* Se verifica si hay que mostrar el promedio */
 					strcpy(lista_promedio,"");
					promedio(parametro, lista_promedio, newsockfd);
					socket_write(newsockfd,lista_promedio);
 				}

 				else if (!strncmp("descargar", comando, 9)){				/* Se verifica si hay que "descargar" */
 					n_estacion = atoi(parametro);
 					if (!n_estacion)
						socket_write(newsockfd,"PARAMETRO INVALIDO");
					else
						descargar(n_estacion);
 				}

 				else {														/* En caso contrario se avisa que se recibio el mensaje */
 					socket_write(newsockfd,"Obtuve su mensaje");
 				} 					
			}
		}
		else {
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
	return 0; 
} 


/*======================FUNCIONES======================*/
/* Lee el socket */
void socket_read(int newsockfd, char buffer[TAM]){
	int n;

	memset( buffer, 0, TAM );				/* Se limpia el buffer */
	n = read( newsockfd, buffer, TAM-1 );	/* Se lee el socket y se copian los datos en el buffer */
	if ( n < 0 ) {							/* Control de errores */
		perror( "lectura de socket" );
		exit(1);
	}

	printf( "PROCESO %d. ", getpid() );
	printf( "Recibí: %s", buffer );
}


/* Escribe el socket */
void socket_write(int newsockfd, char mensaje[TAM]){
	int n;

	n = write( newsockfd, mensaje, strlen(mensaje)+1 );	/* Se escribe en el socket el mensaje */
	if ( n < 0 ) {										/* Control de errores */
		perror( "escritura en socket" );
		printf("error en escribir\n");
		exit( 1 );
	}	
}
