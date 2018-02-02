#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 512

void listar_sensores(void);
void diario_precipitacion(void);
void mensual_precipitacion(void);
void datos(char estacion[5]);

int main(){
	char *token;
	char estacion[5];
	int n_estacion;
	char a[12];
	printf("Content-type: text/html\n\n");
	printf("<html>");

	token = getenv("QUERY_STRING");
	sscanf(token, "%[^=]=%s", a,estacion);

	datos(estacion);
	
	printf("<h3>Sensores de la Estacion %s: </h3>", estacion);
	fflush(0);
	listar_sensores();

    printf("<h3>Precipitacion Mensual: </h3>");
	fflush(0);
	mensual_precipitacion();

	printf("<h3>Precipitacion Diaria: </h3>");
	fflush(0);
	diario_precipitacion();

	printf("<h3>Descarga de Datos </h3>");
	fflush(0);

	printf("<CENTER><p><h3><a href=\"../files/tmp_estacion.txt\" download=\"datos_meteorologicos_%s.txt\">Descargar Datos de la Estacion %s</a></h3></p></CENTER>", estacion, estacion); 
	
	printf("</html>");
}

/*===================================================================================*/
/* BUSCA LA ESTACION, PASADA COMO PARAMETRO, EN LA "BASE DE DATOS" Y ALMACENA LA 
 * INFORMACION EN UN NUEVO ARCHIVO TEMPORAL
*/

void datos(char estacion[]){

	char buffer[TAM];
	char aux_buffer[TAM];
	char *token; 
	const char s[2] = ",";

	FILE *fsource;
	FILE *fdestination;

	fsource=fopen("../files/datos_meteorologicos.CSV", "r");
	fdestination=fopen("../files/tmp_estacion.txt", "w");
	if (fsource == NULL) 
        printf("\nError de apertura del archivo fuente. \n\n");
    else if(fdestination == NULL)
        printf("\nError de apertura del archivo destino. \n\n");

      
    else{
		fgets(buffer,TAM,fsource);
		fputs(buffer,fdestination);
    	fgets(buffer,TAM,fsource);
    	fputs(buffer,fdestination);
    	fgets(buffer,TAM,fsource);
    	fputs(buffer,fdestination);
	
		while (!feof(fsource)){	/* Se recorre el archivo */
			fgets(buffer,TAM,fsource);
			strcpy(aux_buffer,buffer);	
			token=strtok(aux_buffer,s);
			if (!strncmp(estacion,token,5)){
				fputs(buffer,fdestination);
			}
		}
		fclose(fsource);
		fclose(fdestination);
	}	
}

/*===================================================================================*/
/* CREA UN LISTADO DE TODOS LOS SENSORES QUE TIENE LA ESTACION
*/

void listar_sensores(void){

	char buffer[TAM+1];
	char variables[TAM+1];
	int cant_sensores;
	char *token; 
	const char z[2] = ",";
	char *token1;  				
	int cont=0;

	FILE *fstat;

	fstat=fopen("../files/tmp_estacion.txt", "r");
	if (fstat == NULL)
        printf("\nError de apertura del archivo. \n\n");
      
    else{
		fgets(buffer,TAM+1,fstat); /* Cabecera */
    	fgets(buffer,TAM+1,fstat); /* Espacio en blanco */
    	fgets(variables,TAM+1,fstat); /* Se guarda el nombre de las variables */
	
		while (!feof(fstat)){	/* Se recorre el archivo */
			fgets(buffer,TAM+1,fstat);	
			token=strtok(buffer,z);
			cant_sensores=0;					
		  	 /*Se recorre el buffer y se cuentan los sensores que tiene la estacion */
		    while (token != NULL){ 
		       	if (!strncmp(token,"--",2)){
		       		break;
		       	}
		       	cant_sensores=cant_sensores+1;
		       	token = strtok(NULL, z);
		    }
		    cant_sensores = cant_sensores - 4; 

			/* Se imprime el nombre de los sensores de la estación */	
			cont=0;
		    token1 = strtok(variables, z); 
		    while( cont < cant_sensores+4 ){ 
		       if (cont >= 4){
		       		printf("<p><li>%s</li></p>", token1);
		       		fflush(0);
		       	}
		       token1 = strtok(NULL, z);
		       cont = cont+1;
			}
			break;		
		}
		fclose(fstat);
	}	
 }


/*===================================================================================*/
/*CALCULA EL ACUMULADO MENSUAL DE PRECIPITACION*/
void mensual_precipitacion(void){

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

	fstat=fopen("../files/tmp_estacion.txt", "r");
	if (fstat == NULL){
        printf("<p>Error de apertura del archivo. </p>");
        fflush(0);
	}

    else{
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

		sscanf(cabecera, "%[^\n ] %[^\n ] %[^\n ] %[^\n ] %[^\n]",a,b,c,d,e);
		sscanf(d,"%[^\n/]/%[^\n]",dia, aux_mes);
  
		while (!feof(fstat)){
			fgets(buffer,TAM+1,fstat);	
			sscanf(buffer, "%d,%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%f,%[^\n]", &estacion,b,c,fecha,e,f,g,&precipitacion,h);	
			sscanf(fecha, "%[^\n/]/%[^\n ] %[^\n]", dia, mes, hora);

			if (strcmp(aux_mes,mes) == 0){ /* Se chequea que sea el mismo dia */
				acumulado = acumulado+precipitacion; /* Se agrega la medicion al acumulado */
			}
			
			else{
				flag = 0;

				sprintf(aux_acumulado_mensual,"mes: %s precipitacion: %.1f mm",aux_mes, acumulado);	
				printf("<li>%s</li>", aux_acumulado_mensual);
				fflush(0);
													
				sscanf(fecha, "%[^\n/]/%[^\n ] %[^\n]", dia, mes, hora);
				strcpy(aux_mes,mes);
				acumulado = precipitacion; /* Se grega la primera medicion al acumulado */
			}
		}
		sprintf(aux_acumulado_mensual,"mes: %s precipitacion: %.1f mm",aux_mes, acumulado);
		printf("<li>%s</li>", aux_acumulado_mensual);
		fflush(0);

		fclose(fstat);
	}	
}

/*===================================================================================*/
/*CALCULA EL ACUMULADO DIARIO DE PRECIPITACION*/
void diario_precipitacion(void){

	float precipitacion, acumulado;
	int estacion;
	int flag=0;
	char dia[12], aux_dia[12], hora[8], fecha[20], aux_acumulado_diario[40];
	char buffer[TAM];
	char a[20],b[20],c[20],d[20],e[20],f[20],g[20],h[20];
	char cabecera[40];
	char variables[1024];
	FILE *fstat;

	fstat=fopen("../files/tmp_estacion.txt", "r");
	
	if (fstat == NULL){
        printf("<p><h2>Error de apertura del archivo.<h2></p>");
    	fflush(0);
    }
      
    else{
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

		sscanf(cabecera, "%[^\n ] %[^\n ] %[^\n ] %[^\n ] %[^\n]",a,b,c,d,e);
    	strcpy(aux_dia,d);

    	acumulado=0.0;
    	precipitacion=0.0;
	
		while (!feof(fstat)){
			fgets(buffer,TAM+1,fstat);	
			sscanf(buffer, "%d,%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%[^\n,],%f,%[^\n]", &estacion,b,c,fecha,e,f,g,&precipitacion,h);
			sscanf(fecha, "%[^\n ] %[^\n]", dia, hora);

			if (strcmp(aux_dia,dia) == 0){ 				/* Se chequea que sea el mismo dia */
				acumulado = acumulado+precipitacion; 	/* Se agrega la medicion al acumulado */
			}
			
			else{
				sprintf(aux_acumulado_diario,"dia: %s precipitacion: %.1f mm",aux_dia, acumulado);
				printf("<li>%s</li>", aux_acumulado_diario);
				fflush(0);	
													
				sscanf(fecha, "%[^\n ] %[^\n]", dia, hora);
				strcpy(aux_dia,dia);
				acumulado = precipitacion; 	/* Se agrega la primera medicion al acumulado */
			}

		}
		sprintf(aux_acumulado_diario,"dia: %s precipitacion: %.1f mm",aux_dia, acumulado);
		printf("<li>%s</li>", aux_acumulado_diario);
		fflush(0);
	}
	fclose(fstat);
}
