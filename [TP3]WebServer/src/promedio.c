#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM 256
void promedio(char variable[4]);
int main(){
	char *token;
	char variable[4];
	char a[12];
	printf("Content-type: text/html\n\n");
	printf("<html>");

	token = getenv("QUERY_STRING");
	sscanf(token, "%[^=]=%s", a,variable);
	promedio(variable);
	printf("</html>");
}

void promedio (char variable[]){
	char estacion[5]="";
	char aux_estacion[5]="";
	int cont, posicion, cont_token;
	char aux_promedio[64]="";
	float valor;
	float promedio;
	char b[24],medida[10]="";
	char *token; 
	const char s[2] = ",";
	char buffer[TAM+1]="";
	char cabecera[40]="";
	char variables[512]="";
	FILE * fstat;

	fstat=fopen("../files/datos_meteorologicos.CSV", "r");
	if (fstat == NULL){
        printf("<p>Error de apertura del archivo</p>");
        fflush(0);
    }
      
    else{
    	posicion = atoi(variable);
    	promedio = 0.0;
    	strcpy(medida,"");
		
		fgets(cabecera,TAM+1,fstat);
    	fgets(buffer,TAM+1,fstat);
    	fgets(variables,TAM+1,fstat);

    	/* recorrer las variables y guardar la medida*/
	    token = strtok(variables, s); /* get the first token*/
	    cont_token=1;
	    while( token != NULL ){	/*walk through other tokens*/
	       if (cont_token == posicion){
	       		sscanf(token, "%s %[^\n]",b,medida);
	       		break;
	       }
	       cont_token=cont_token+1;
	       token = strtok(NULL, s);
	    }

	    if(token == NULL){
	    	printf("<p>ERROR: variable no encontrada</p>");
	    	fflush(0);
	    }
	    else{
			cont=0;
			/* TOMO EL PRIMER VALOR DE LA VARIABLE Y EL NUMERO DE ESTACION DE LA PRIMERA ESTACION PARA COMPARAR MAS ADELANTE*/
			fgets(buffer,TAM+1,fstat);
	
	    	token = strtok(buffer, s); /* get the first token*/
	    	cont_token=1;
	    	strcpy(aux_estacion,token);
	    	while (token != NULL){ 
	    		if ((cont_token == posicion)&&(strcmp(token,"--"))){
	    			promedio= atof(token);
	    			cont=1;
	    		}
		      	cont_token=cont_token+1;
		       	token = strtok(NULL, s);
		    }
			

			while (!feof(fstat)) {
				fgets(buffer,TAM+1,fstat);	
				token = strtok(buffer, s); /* get the first token*/
				strcpy(estacion,token);
				valor=0.0;

				/*Busco el valor de la variable*/
				cont_token=1;
		    	while (token != NULL){ 
		    		if ((cont_token == posicion)&&(strcmp(token,"--"))){
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
					if (cont>1){
						promedio = promedio /cont;
						sprintf(aux_promedio,"Estacion %s Promedio: %.2f %s", aux_estacion, promedio, medida);
						printf("<li>%s</li>", aux_promedio);
						fflush(0);
					}

					strcpy(aux_estacion,estacion);
					promedio=valor;
					cont=0;	
				}			
			}
			if (cont>1)
				promedio = promedio /cont;
			sprintf(aux_promedio,"Estacion %s Promedio: %.2f %s", estacion, promedio, medida);
			printf("<li>%s</li>", aux_promedio);
			fflush(0);
			fclose(fstat);
		}
	}	
}
