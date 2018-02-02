/*
 * Programa: Cliente
 * Familia: AF_INET -> Conexion Internet
 * Tipo: SOCK_STREAM -> Orientado a Conexion FULL DUPLEX con control de errores 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 
#define TAM 1400

void socket_read(int sockfd, char buffer[TAM]); /* Lee el socket */
void socket_write(int sockfd, char buffer[TAM]); /* Escribe el socket */
int login (char usuario[20], char ip[12], int sockfd, char buffer[TAM]); /* Inicio de sesion */
int conexion(char ip[12], char port[4]); /* Se crea la conexion TCP */
void descargar(char ip[12], char port[4]); /* Descarga el archivo de datos */
void help(); /*Lista los comandos aceptados*/

int main( int argc, char *argv[] ) {
	int sockfd; /* sockfd almacena el descriptor del socket */
	int n;		/* n sirve para control de error a la hora de escribir o leer */
	int log = 0;  
	char buffer[TAM];
	char promp[20]="";
	char comand[7],	usuario[20], ip[12], port[4];
			
	while(1) {
		
		printf( ">>: " );				/* Se imprime promp simple */
		memset( buffer, '\0', TAM );	/* Se limpia el buffer	*/
		fgets( buffer, TAM-1, stdin );	/* Se guarda la entrada de teclado en el buffer	*/

		buffer[strlen(buffer)-1] = '\0';
		
		if(!strcmp("fin", buffer))		/* Si se escribe "fin" finaliza el programa */
		{
			n = write( sockfd, buffer, strlen(buffer) );	/*Si hay una conexion abierta se envia un mensaje para avisar */
			if ( n < 0 ) {
				perror( "escritura de socket" );
				exit( 1 );
			}
			printf( "Finalizando ejecución\n" );
			exit(0);
		}

		if(!strcmp("help", buffer)){
			help();
		}
	

		if ((strstr(buffer," ") != NULL) && (strstr(buffer,"@") != NULL) && (strstr(buffer,":") != NULL)){	/* Si el comando contiene " ","@" y ":" se parsea */
		//if( !strncmp( "connect", comand, 7 ) ){
		//if( !strncmp( "connect", buffer, 7 ) ){
			//strcpy(buffer,"connect user1@192.168.1.107:6020");
			sscanf(buffer, "%[^\t\n ] %[^\t\n@]@%[^\t\n:]:%[^\t\n]",comand,usuario,ip,port);
		
			if( !strncmp( "connect", comand, 7 ) ){	/* Si la primer parte del comando es connect creamos una conexion */
				sockfd = conexion(ip, port);		/* Se genera una conexion a esa ip */
				if (sockfd >= 0){
					log = login(usuario, ip, sockfd, buffer);	/* Iniciar sesion */
					if (log)	/* Si el logueo fue correcto se genera el promp correspondiente */
						sprintf(promp,"%s@%s$ ",usuario,ip);			
				}
			}
		}
		
		while (log){						/* Mientras hay una sesion iniciada	*/
			printf("%s", promp);			/* Se imprime el promp */
			socket_write(sockfd, buffer);	/*Se escribe en el socket el contenido del buffer */
			
			buffer[strlen(buffer)-1] = '\0';
			if( !strcmp( "desconectar", buffer))	/* Se verifica si se escribio desconectar */
				log = 0;	/* Cambio la vandera log para salir del bucle */

			else if((strlen(buffer)==15) && (!strncmp("descargar", buffer,9)) ){	/*Se verifica si se escribio descargar */
				descargar(ip, port);	/* Se llama a la funcion descargar */
				printf("Datos Descargados\n");
			} 

			else 
				socket_read(sockfd, buffer);	/* Se lee el socket */
		}		
	}
	return 0;
} 


/*======================FUNCIONES======================*/
// Lee el socket
void socket_read(int sockfd, char buffer[TAM]){
	int n;
	memset(buffer, '\0', TAM);		/* Se limpia el buffer */
	n = read(sockfd, buffer, TAM);	/* Se lee el socket */
	if ( n < 0 ) {					/* Control de error */
		perror( "lectura de socket" );
		exit( 1 );
	}
	printf( "%s\n", buffer );		/* Se imprime el buffer */
}

/*========================================================================================*/

/* Escribe el socket */
void socket_write(int sockfd, char buffer[TAM]){
	int n;
	memset( buffer, '\0', TAM );	/* Se limpia el buffer */
	fgets( buffer, TAM-1, stdin );	/* Se escribe lo ingresado por teclado en el buffer */

	n = write( sockfd, buffer, strlen(buffer) );	/* Se escribe el socket */
	if ( n < 0 ) {	/* Control de error */
		perror( "escritura de socket" );
		exit( 1 );
	}
}

/*========================================================================================*/

/* Genera la conexion con otro socket */ 
int conexion(char ip[], char port[]){
	int sockfd, puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	puerto = atoi(port); /* NUMERO DE PUERTO */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	/* AF_INET es para usar los protocolos ARPA de Internet */
												/* SOCK_STREAM es para usar la conexion TCP */
	if ( sockfd < 0 )	/* Control de error */ 
		perror( "ERROR apertura de socket" );

	else{
		server = gethostbyname(ip); /* gethostbyname() devuelve un puntero a la estructura struct hostent que se ha llenado con la ip que ingreso el usuario */
		if (server == NULL) 
			fprintf( stderr,"Error, no existe el host\n" );
		else{
			memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
			serv_addr.sin_family = AF_INET;
			bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length ); 	/* void bcopy(const void *src, void *dest, size_t n) */
																									/* copia la direccion ip del server en la estructura del socket*/
			serv_addr.sin_port = htons( puerto ); /* Guarda el puerto en la structura del socket */
			if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 )	/* Se crea la conexion */
				perror( "conexion" );
			else
				return sockfd;
		}
	}
	return -1;		
}

/*========================================================================================*/

//Inicio de Sesion
int login (char usuario[], char ip[], int sockfd, char buffer[]){
	int n;
	n = write( sockfd, buffer, strlen(buffer) ); /* Se vuelve a escribir en el socket el comando para iniciar sesion */
	if ( n < 0 ) {
		perror( "escritura de socket" );
		exit( 1 );
	}
	socket_read(sockfd, buffer);	/* En este punto el server pedira password */
	socket_write(sockfd, buffer); 
	socket_read(sockfd,buffer);

	if(!strcmp("login successful", buffer)){	/* Si el inicio de sesion fue exitoso se imprimen los posibles comandos */
		printf("help -> para listar los comandos aceptados por el servidor\n");
		return 1;
	}
	return 0;				
}	


/*========================================================================================*/
/* Esta funcion descarga los datos del servidor en un archivo datos.txt 
 * Si el archivo datos.txt no existe lo crea. 
 * Para realizar la descarga se crea una conexion UDP con el servidor 
*/
void descargar(char ip[], char port[]){
	int udp_sockfd, puerto, n; //tamano_direccion;
	socklen_t tamano_direccion;
	struct sockaddr_in dest_addr;
	struct hostent *server;
	char buffer[TAM];
	puerto = 6020;

	server = gethostbyname(ip);	/* Puntero a la estructura struct hostent que contiene la ip del servidor */
	if ( server == NULL ) {
		fprintf( stderr, "ERROR, no existe el host\n");
		exit(0);
	}

	udp_sockfd = socket( AF_INET, SOCK_DGRAM, 0 );	/* AF_INET es para usar los protocolos ARPA de Internet */
													/* SOCK_DGRAM es para usar la conexion UDP */
	if (udp_sockfd < 0) {
		perror( "apertura de socket" );
		exit( 1 );
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(puerto);	/* Se guarda el numero de puerto en la estructura del socket */
	dest_addr.sin_addr = *((struct in_addr *)server->h_addr);	/*Se almacena la IPv4 en la estructura del socket */
	memset( &(dest_addr.sin_zero), '\0', 8 );	/*Se rellena la estructura para que sea del mismo tamaño que la estructura sockaddr */

	printf( "Presione cualquier tecla para comenzar la descarga" );
	memset( buffer, 0, TAM );
	fgets( buffer, TAM, stdin );

	tamano_direccion = sizeof( dest_addr );
	n = sendto( udp_sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, tamano_direccion );
	if ( n < 0 ) {
		perror( "Escritura en socket" );
		exit( 1 );
	}
	memset( buffer, 0, sizeof( buffer ) );

	FILE *fdatos;
	fdatos = fopen("./datos.txt", "w");	/* Se abre el archivo datos.txt, si no existe se crea */

	while (1){ 
		memset( buffer, 0, sizeof( buffer ) );	/* Se limpia el buffer */
		n = recvfrom( udp_sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
		printf("%s\n", buffer );
		fputs(buffer,fdatos);
		if ( n < 0 ) {
			perror( "Lectura de socket" );
			exit( 1 );
		}
		if (!strncmp( "terminar", buffer, 8 )){ //si llega "terminar" se sale del bucle
			break;
		}
	}
	printf("Finalizo la transmisión de datos\n");
	fclose(fdatos);	/* Se cierra el archivo */
	close(udp_sockfd);	/* Se cierra el Socket UDP */
	printf("Finalización de conexión UDP\n");
}

/*========================================================================================*/
/* Esta funcion lista los comandos válidos */
void help (){
	printf("-------------------------COMANDOS ACEPTADOS------------------\n");
	printf("connect usuario@ip:puerto -> para iniciar una conexión\n");
	printf("desconectar -> para terminar una sesion \n");
	printf("fin -> para finalizar el programa\n");
	printf("-------------------------------------------------------------\n"); 
}