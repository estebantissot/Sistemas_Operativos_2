#define TAM 1400


/*===================================================================================*/
/* LISTA LOS COMANDOS PERMITIDOS */

void listar_comandos(char lista_comandos[]){
	strcat(lista_comandos, "-------------------------COMANDOS ACEPTADOS-------------------------\n");
	strcat(lista_comandos, "desconectar -> termina la sesion \n");
	strcat(lista_comandos, "listar -> lista todas las estaciones que hay en la base de datos y de que sensores tiene datos\n");
	strcat(lista_comandos, "descargar {n°_estacion} -> descarga un archivo con todos los datos de n°_estacion\n");
	strcat(lista_comandos, "diario_precipitacion {n°_estacion} -> muestra el acumulado diario de la variable precipitacion\n");
	strcat(lista_comandos, "mensual_precipitacion {n°_estacion} -> muestra el acumulado mensual de la variable precipitacion\n");
	strcat(lista_comandos, "promedio {variable} -> muestra el promedio de todas las muestras de la variable de cada estacion\n");
	strcat(lista_comandos, "--------------------------------------------------------------------\n"); 
}


/*===================================================================================*/
/* CREA UN LISTADO DE TODAS LAS ESTACIONES QUE HAY EN LA "BASE DE DATOS"
 * Y MUESTRA DE QUE SENSORES TIENE DATOS
*/

void listar(char lista[]){
	char buffer[TAM+1];
	char estacion[6];
	char aux_estacion[6]="";
	char *nombre;
	char cabecera[40];
	char variables[1024];
	int cant_sensores;
	char lista_sensores[256];
	char aux_string[64]="";
	char *token; 
	const char z[2] = ",";
	char *token1;  					/* Se guarda el nombre de los sensores correspondientes a esta estacion. */
	char aux[1024];
	int cont=0;
	FILE *fstat;

	fstat=fopen("./datos_meteorologicos.CSV", "r");
	if (fstat == NULL)
        printf("\nError de apertura del archivo. \n\n");
      
    else{	
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

    	nombre=NULL;
	
		while (!feof(fstat)){	/* Se recorre el archivo */
			fgets(buffer,TAM+1,fstat);	
			token=strtok(buffer,z);
			strcpy(estacion,token); /* Se almacena el ID de la estación*/
			nombre=strtok(NULL, z); /* Se almacena el nombre de la estación*/

			if (!strncmp(aux_estacion,estacion,5)){}	
			else{	/* Se analizan los datos de cada estación */
				strcpy(aux_estacion,estacion);			/* Se guarda el ID de la estacion en una variable auxiliar */
				cant_sensores=0;
					
			  	/* Se recorre el buffer y se cuentan los sensores que tiene la estacion */
			    while (token != NULL){ 
			       	if (!strncmp(token,"--",2)){
			       		break;
			       	}
			       	cant_sensores=cant_sensores+1;
			       	token = strtok(NULL, z);
			    }
			    cant_sensores = cant_sensores - 3; 

			    /* Se guarda la informacion general de la estacion en la lista*/
			    strcpy(aux_string,"");
				sprintf(aux_string,"\nEstacion: %s - %s\nCantidad de sensores: %d\n",estacion, nombre, cant_sensores);
				strcat(lista, aux_string);

				/* Se guarda el nombre de los sensores de la estación en la lista */
			    strcpy(lista_sensores,"");
				strcpy(aux,variables);
				cont=0;
			    token1 = strtok(aux, z); /* Se obtiene el primer token */
			    while( cont < cant_sensores+4 ){ /* Se recorre el resto de los token */
			       if (cont >= 4){
			       		strcat(lista_sensores,token1);
			       		strcat(lista_sensores,"\n");
			       	}
			       token1 = strtok(NULL, z);
			       cont = cont+1;
				}
			    strcat(lista,lista_sensores);
			    printf("lista: %s\n", lista);	
			}
		}
		fclose(fstat);
		printf("cierro el archivo\n");
	}	
}


/*===================================================================================*/
/* CALCULA EL PROMEDIO DE TODAS LAS MUESTRAS DE LA VARIABLE DE CADA ESTACION */

void promedio (char variable[], char lista_promedio[], int newsockfd){

	char estacion[5];
	char aux_estacion[5];
	int cont, posicion, cont_token;
	char aux_promedio[64];
	float valor;
	float promedio;
	char b[24],medida[10];
	char *token; 
	const char s[2] = ",";
	char buffer[TAM+1];
	char cabecera[40];
	char variables[512];
	FILE * fstat;

	fstat=fopen("./datos_meteorologicos.CSV", "r");
	if (fstat == NULL){
        printf("\nError de apertura del archivo \n\n");
    	strcpy(lista_promedio,"\nError de apertura del archivo\n");
    }
      
    else{
    	cont = 0;
    	posicion = 0;
    	promedio = 0.0;
		
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

    	/* recorrer las variables y detectar en que posicion se encuentra*/	
	    token = strtok(variables, s); /* get the first token*/
	    while( token != NULL ){	/*walk through other tokens*/
	       if (!strncmp(token,variable,strlen(variable))){
	       		sscanf(token, "%s %s",b,medida);
	       		break;
	       }
	       posicion=posicion+1;
	       token = strtok(NULL, s);
	    }
	    printf("medida: %s\n", medida );

	    if(token == NULL){
	    	printf("\nERROR: variable no encontrada\n");
	    	strcpy(lista_promedio,"\nERROR: variable no encontrada\n");
	    }
	    else{
		    posicion = posicion;
			
			/* TOMO EL PRIMER VALOR DE LA VARIABLE Y EL NUMERO DE ESTACION DE LA PRIMERA ESTACION PARA COMPARAR MAS ADELANTE*/
			fgets(buffer,TAM+1,fstat);
			printf("%d\n", posicion);
			cont_token=0;
	    	token = strtok(buffer, s); /* get the first token*/
	    	strcpy(aux_estacion,token);
	    	while (token != NULL){ 
	    		if (cont_token == posicion){
	    			valor= atof(token);
	    		}
		      	cont_token=cont_token+1;
		       	token = strtok(NULL, s);
		    }

		    promedio = promedio + valor;
			cont=cont+1;
			printf("valor: %f promedio: %f cont: %d \n",valor,promedio,cont );
			
			while (!feof(fstat)) {
				fgets(buffer,TAM+1,fstat);	
				token = strtok(buffer, s); /* get the first token*/
				strcpy(estacion,token);

				/*Busco el valor de la variable*/
				cont_token=0;
		    	while (token != NULL){ 
		    		if (cont_token == posicion){
		    			valor= atof(token);
		    		}
			      	cont_token=cont_token+1;
			       	token = strtok(NULL, s);
			    }

			    /*Si el el sensor pertenece a la misma estacion que la iteracion anterior lo promedio, si no reinicio el ciclo*/
				if (!strcmp(estacion, aux_estacion)){ 
				    promedio = promedio + valor;
					cont=cont+1;
				}

				else{
					if (cont>0)
						promedio = promedio /cont;
					sprintf(aux_promedio,"Estacion %s Promedio: %.2f %s\n", aux_estacion, promedio, medida);
					printf("aux_promedio: %s",aux_promedio);
					strcat(lista_promedio, aux_promedio);
					strcpy(aux_estacion,estacion);

					promedio=valor;
					cont=0;
					
				}			
			}
			if (cont>0)
				promedio = promedio /cont;
			sprintf(aux_promedio,"Estacion %s Promedio: %.2f %s\n", estacion, promedio, medida);
			strcat(lista_promedio, aux_promedio);
			fclose(fstat);
			printf("Fin de funcion promedio\n");
		}
	}	
}



/*===================================================================================*/
/*CALCULA EL ACUMULADO MENSUAL DE PRECIPITACION DE LA ESTACION PASADA COMO PARAMETRO*/
void mensual_precipitacion(int n_estacion, char precipitacion_mensual[]){

	float precipitacion = 0;
	float acumulado= 0;
	int estacion;
	int flag=0;
	char dia[4], mes[8], aux_mes[12], hora[8], fecha[20], aux_acumulado_mensual[40];
	char buffer[TAM+1];
	char a[20],b[20],c[20],d[20],e[20],f[20],g[20],h[20];
	char cabecera[40];
	char variables[512];
	FILE *fstat;

	fstat=fopen("./datos_meteorologicos.CSV", "r");
	if (fstat == NULL){
        printf("\nError de apertura del archivo. \n\n");
        strcpy(precipitacion_mensual,"\nError de apertura del archivo. \n\n" );
	}

	else if (!n_estacion)
		strcpy(precipitacion_mensual,"PARAMETRO INVALIDO");

    else{
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

		sscanf(cabecera, "%[^\n ] %[^\n ] %[^\n ] %[^\n ] %[^\n]",a,b,c,d,e);
		sscanf(d,"%[^\n/]/%[^\n]",dia, aux_mes);
  
	
		while (!feof(fstat)){
			fgets(buffer,TAM+1,fstat);	
			sscanf(buffer, "%d,%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%f,%[^\n]", &estacion,b,c,fecha,e,f,g,&precipitacion,h);
		  		
			if (n_estacion == estacion){	/* Se chequea numero de estacion */
				flag = 1;
				sscanf(fecha, "%[^\n/]/%[^\n ] %[^\n]", dia, mes, hora);


				if (strcmp(aux_mes,mes) == 0){ /* Se chequea que sea el mismo dia */
					acumulado = acumulado+precipitacion; /* Se agrega la medicion al acumulado */
				}
				
				else{
					flag = 0;

					sprintf(aux_acumulado_mensual,"mes: %s precipitacion: %.1f mm\n",aux_mes, acumulado);
					strcat(precipitacion_mensual, aux_acumulado_mensual);	
														
					sscanf(fecha, "%[^\n/]/%[^\n ] %[^\n]", dia, mes, hora);
					strcpy(aux_mes,mes);
					acumulado = precipitacion; /* Se grega la primera medicion al acumulado */
				}

			}
		}
		if (flag){
			sprintf(aux_acumulado_mensual,"mes: %s precipitacion: %.1f mm\n",aux_mes, acumulado);
			strcat(precipitacion_mensual, aux_acumulado_mensual);
		}
		fclose(fstat);
	}	
}




/*===================================================================================*/
/*CALCULA EL ACUMULADO DIARIO DE PRECIPITACION DE LA ESTACION PASADA COMO PARAMETRO*/
void diario_precipitacion(int n_estacion, char acumulado_diario[]){
	float precipitacion, acumulado;
	int estacion;
	int flag=0;
	char dia[12], aux_dia[12], hora[8], fecha[20], aux_acumulado_diario[40];
	char buffer[TAM];
	char a[20],b[20],c[20],d[20],e[20],f[20],g[20],h[20];
	char cabecera[40];
	char variables[1024];
	FILE *fstat;

	fstat=fopen("./datos_meteorologicos.CSV", "r");
	if (!n_estacion)
		strcpy(acumulado_diario,"PARAMETRO INVALIDO");

	else if (fstat == NULL){
        printf("\nError de apertura del archivo. \n\n");
    	strcpy(acumulado_diario,"\nError de apertura del archivo. \n\n" );
    }
      
    else{
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

		sscanf(cabecera, "%[^\n ] %[^\n ] %[^\n ] %[^\n ] %[^\n]",a,b,c,d,e);
    	strcpy(aux_dia,d);
  
	
		while (!feof(fstat)){
			fgets(buffer,TAM+1,fstat);	
			sscanf(buffer, "%d,%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%f,%[^\n]", &estacion,b,c,fecha,e,f,g,&precipitacion,h);
		  		
			if (n_estacion == estacion){					/* Se chequea el numero de estacion */
				flag = 1;
				sscanf(fecha, "%[^\n ] %[^\n]", dia, hora);

				if (strcmp(aux_dia,dia) == 0){ 				/* Se chequea que sea el mismo dia */
					acumulado = acumulado+precipitacion; 	/* Se agrega la medicion al acumulado */
				}
				
				else{

					sprintf(aux_acumulado_diario,"dia: %s precipitacion: %.1f mm\n",aux_dia, acumulado);
					strcat(acumulado_diario, aux_acumulado_diario);	
														
					sscanf(fecha, "%[^\n ] %[^\n]", dia, hora);
					strcpy(aux_dia,dia);
					acumulado = precipitacion; 	/* Se agrega la primera medicion al acumulado */
				}

			}
			else {	
				if (flag){
					sprintf(aux_acumulado_diario,"dia: %s precipitacion: %.1f mm\n",aux_dia, acumulado);
					strcat(acumulado_diario, aux_acumulado_diario);
					break;
				}
			}
		}
	}
	fclose(fstat);
}



/*===================================================================================*/
/* INICIA UNA CONEXION UDP Y TRANSFIERE TODOS LOS DATOS DEL ARCHIVO */
void descargar(int n_estacion){
	int udp_sockfd, n; /* El entero udp_sockfd almacena el descriptor del socket */
	socklen_t tamano_direccion;
	char buffer[TAM];
	struct sockaddr_in serv_addr;
	int puerto = 6020;
	int estacion = 0;
	char str_aux[128];
	FILE *fstat;

	udp_sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	if (udp_sockfd < 0) { 
		perror("ERROR en apertura de socket");
		exit( 1 );
	}

	memset( &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	/*INADDR_ANY indica que se selecione automáticamente la dirección IP de la máquina sobre la que está ejecutando el proceso */
	serv_addr.sin_port = htons( puerto ); /* Se Guarda el puerto en la structura del socket */
	memset(&(serv_addr.sin_zero), '\0', 8);

	if( bind( udp_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) { /* Se asocia un socket con el puerto */
		perror( "ERROR en binding" );
		exit( 1 );
	}

    printf( "Socket disponible: %d\n", ntohs(serv_addr.sin_port) );

	tamano_direccion = sizeof( struct sockaddr );
	memset( buffer, 0, TAM );
	n = recvfrom( udp_sockfd, buffer, TAM-1, 0, (struct sockaddr *)&serv_addr, &tamano_direccion );
	if ( n < 0 ) {
		perror( "lectura de socket" );
		exit( 1 );
	}

/*------------------------------------------------------------------------------------------------------*/
	

	fstat=fopen("./datos_meteorologicos.CSV", "r");
	if (fstat == NULL){
    	printf("\nError de apertura del archivo. \n\n");
	}
	else{
		fgets(buffer,TAM+1,fstat);	/* Se envia la cabecera*/
		n = sendto( udp_sockfd, (void *)  buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		if ( n < 0 ) {
			perror( "escritura en socket" );
			exit( 1 );
		}
    	fgets(buffer,TAM+1,fstat);	/* Se pierde la fila vacia */
    	fgets(buffer,TAM+1,fstat);	/* Se envia el nombre de las variables */
    	n = sendto( udp_sockfd, (void *)  buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		if ( n < 0 ) {
			perror( "escritura en socket" );
			exit( 1 );
		}

		while (!feof(fstat)){
			fgets(buffer,TAM+1,fstat);
			sscanf(buffer,"%d,%s",&estacion, str_aux);
			if (n_estacion == estacion){	/* Se verifica que los datos sigan siendo de estacion solicitada */
				n = sendto( udp_sockfd, (void *)  buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
				if ( n < 0 ) {
					perror( "escritura en socket" );
					exit( 1 );
				}
			}
		}
		memset( buffer, 0, TAM );
		n = sendto( udp_sockfd, (void *) "terminar", 8, 0, (struct sockaddr *)&serv_addr, tamano_direccion  ); /* Se avisa que finaliza la transmision de datos */
		if ( n < 0 ) {
			perror( "escritura en socket" );
			exit( 1 );
		}
	}

	fclose(fstat);
	close(udp_sockfd); /* Se cierra la conexion UDP */
	printf("Finalizacion de Conexion UDP\n");
}
