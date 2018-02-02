#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM 512

int main(){
	printf("Content-type: text/html\n\n");
	printf("<html>");

	char buffer[TAM+1];
	char estacion[6];
	char aux_estacion[6]="";
	char *nombre;
	char cabecera[40];
	char variables[1024];
	int cant_sensores;
	char aux_string[64]="";
	char *token; 
	const char z[2] = ",";

	FILE *fstat;

	fstat=fopen("../files/datos_meteorologicos.CSV", "r");
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
			    printf("<CENTER>");
			    printf("<h2>");
			    printf("<li>");
				sprintf(aux_string,"\nEstacion: %s - %s\n",estacion, nombre);
				printf("%s\n", aux_string);
				printf("</li>");
				printf("</h2>");
				printf("</CENTER>");
				fflush(0);
				printf("<CENTER>");
				printf("<h3>");
				printf("Cantidad de Sensores: %d\n", cant_sensores);
				printf("</h3>");
				printf("</CENTER>");
				fflush(0);
			}
		}
		fclose(fstat);
	}	
}
