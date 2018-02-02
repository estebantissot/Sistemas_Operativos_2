#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#define m_gate 10 /*muestras por gate*/

int n_pulsos(int *cantidad_pulsos, uint16_t *cant_max_muestras);
int pulsos(int cantidad_pulsos, float (*matriz_v)[cantidad_pulsos], float (*matriz_h)[cantidad_pulsos]);
void auto_correlacion(int i, int cantidad_pulsos, float autocorr[], float (*matriz)[cantidad_pulsos]);
int escribir_archivo(int tam, float autocorr[], char nombre_archivo[]);

int main (int argc, char *argv[]){
	//const int m_gate=10; /*muestras por gate*/

	double ti, tf;
	int i,j;
	int cantidad_pulsos=0;
	uint16_t cant_max_muestras=0;
	uint16_t cant_max_gates=0;
	int num_threads;
	int num_th_write;

	
	if (argc == 1){
		printf("ERROR: NO se indicó la cantidad de hilos. \n    EJ: ./radar 2\n");
		return 0;
	}
	else {num_threads = atoi(argv[1]);}

	if (argc == 2 && num_threads == 1){
		num_th_write = 1;
		printf("Ejecucion Procedural\n");
	}
	else{
		num_th_write = 2;
		printf("Ejecucion en Paralelo con %d hilos de %d disponibles\n", num_threads, omp_get_max_threads());
	}
	

	n_pulsos(&cantidad_pulsos, &cant_max_muestras); /*tamaño de matriz*/

	/* Se Definen matrices y vectores con los datos calculados en la funcion anterior*/
	cant_max_gates = cant_max_muestras/m_gate;
	float matriz_v[cant_max_gates][cantidad_pulsos]; /* Matriz(gate,pulso) del canal V */
	float matriz_h[cant_max_gates][cantidad_pulsos]; /* Matriz(gate,pulso) del canal H */
	float autocorr_V[cant_max_gates]; /* Vector de autocorrelacion del canal V */
	float autocorr_H[cant_max_gates]; /* Vector de autocorrelacion del canal H */
	
	ti= omp_get_wtime(); /* Se guarda el tiempo actual */

	#pragma omp parallel for private(j) collapse(2) num_threads(num_threads)
	for (i=0; i<cantidad_pulsos; i++){ /* Se inicializa la matriz */
		for (j=0; j<cant_max_gates; j++){
			matriz_v[j][i]=0.0;
			matriz_h[j][i]=0.0;
		}
	}

	pulsos(cantidad_pulsos, matriz_v, matriz_h); /* Se llena la matriz (gate,pulso) de cada canal*/

	#pragma omp parallel for private(i) shared(cantidad_pulsos,autocorr_V,matriz_v) num_threads(num_threads) /* Se realiza la autocorrelacion, en forma paralela, del canal vertical*/
		for (i=0; i<cant_max_gates; i++){
			auto_correlacion(i, cantidad_pulsos,autocorr_V, matriz_v);
			//printf("Cantidad de threads dentro de for1: %d\n", omp_get_num_threads());
	 	}

	#pragma omp parallel for private(i) shared(cantidad_pulsos,autocorr_H,matriz_h) num_threads(num_threads) /* Se realiza la autocorrelacion, en forma paralela, del canal horizontal*/
		for (i=0; i<cant_max_gates; i++){ 
	 		auto_correlacion(i, cantidad_pulsos,autocorr_H, matriz_h);
	 		//printf("Cantidad de threads dentro de for2: %d\n", omp_get_num_threads());
	 	}

	#pragma omp parallel sections num_threads(num_th_write)/* Se definen dos secciones en paralelo para escribir los resultados*/
	{
	 	#pragma omp section /* Se escribe la autocorrelacion del canal V en un archivo binario. */
	 	{
	 		escribir_archivo(cant_max_gates, autocorr_V, "./autocorrelacion_canal_V.iq");
	 		//printf("Hilo: %d escribio \"autocorrelación_canal_V.iq\"\n", omp_get_thread_num());
	 	}

		#pragma omp section /* Se escribe la autocorrelacion del canal H en un archivo binario. */
	 	{
		 	escribir_archivo(cant_max_gates, autocorr_H, "./autocorrelacion_canal_H.iq");
	 		//printf("Hilo: %d escribio \"autocorrelación_canal_H.iq\"\n", omp_get_thread_num());
	 	}
	} 

	tf = omp_get_wtime(); /* Se guarda el tiempo actual */
	printf("Tiempo: %lf\n",tf-ti);

	return 0;
}




/*========================================================================================*/
/* Esta funcion aplica la formula de autocorrelación en tiempo discreto */
void auto_correlacion(int i, int cantidad_pulsos, float autocorr[],float (*matriz)[cantidad_pulsos]){
	float op=0.0;

	for (int j=0; j<cantidad_pulsos; j=j+2){ //afuera
		op = op + (matriz[i][j] * matriz[i][j+1]);
	}
	autocorr[i] = op/cantidad_pulsos;
	//printf("id_hilo autocorrelacion: %d i:%d valor: %f\n", omp_get_thread_num(), i, op/cantidad_pulsos);
}

/*========================================================================================*/
/* Esta funcion escribe el vector de entrada en un archivo binario */
int escribir_archivo(int tam, float autocorr[], char nombre_archivo[]){
	FILE * fp;

	fp = fopen(nombre_archivo, "w+b"); /* Se abre el archivo autorrelacion.iq para escritura, si no existe se crea */
	if (fp==NULL){  /* Control de errores */
		printf("ERROR: el archivo no se pudo abrir/crear");
		return 0;
	}
	else{
		for (int i=0; i<tam; i++){
			//printf("id hilo: %d escribiendo archivo: %s\n", omp_get_thread_num(), nombre_archivo);
			fwrite(&autocorr[i],1,sizeof(float),fp); /* Se escribe el archivo binario */
		}
		fclose(fp); /* Se cierra el archivo */
	}
	return 0;
}

/*========================================================================================*/
/* Esta funcion retorna la cantidad de pulsos y cantidad maxima de muestras
 * que hay en el archivo binario./pulsos.ip
 */
int n_pulsos(int *cantidad_pulsos, uint16_t *cant_max_muestras){
	uint16_t cant_muestras;
	FILE *fp;

	fp = fopen("./pulsos.iq","r+b");
	if (fp==NULL){ 
		printf("ERROR: el archivo no se pudo abrir");
		return 0;
	}	
	else{
		*cantidad_pulsos = 0;
		fread(&cant_muestras,2,1,fp);
		*cant_max_muestras = cant_muestras;
		while (!feof(fp)){
			fseek(fp,2*cant_muestras*2*sizeof(float), SEEK_CUR); /*desplazo el puntero del fichero n cantidad de bytes, n = 2(canal V y canal H) * cantidad de muestras * 2 (cada muestra esta compuesta por dos floats) * sizeof(float) la cantidad de bytes que ocupa un float */
			fread(&cant_muestras,2,1,fp);
			if(cant_muestras > *cant_max_muestras) 
				*cant_max_muestras=cant_muestras;
			*cantidad_pulsos= *cantidad_pulsos+1;
		}
		fclose(fp);
	}
	return 0;
}


/*========================================================================================*/
/* Esta funcion devuelve una matriz(gate,pulso) por canal */
int pulsos(int cantidad_pulsos, float (*matriz_v)[cantidad_pulsos], float (*matriz_h)[cantidad_pulsos]){
	struct Samples{
		uint16_t cantidad_muestras;
		float a;
		float b;
	};

	int pulso;
	int cantidad_gates;
	int muestras_sobrantes;
	float sum_gate;
	struct Samples vs1={0,0.0,0.0};

	FILE *fp;
	fp = fopen("./pulsos.iq","r+b");
	if (fp==NULL){ 
		printf("ERROR: el archivo no se pudo abrir");
		return 0;
	}
	
	else{
		cantidad_gates=0;
		muestras_sobrantes=0;
		pulso=0;

		fread(&vs1.cantidad_muestras,2,1,fp); /* Se toma la cantidad de muestras del primer pulso */
		while (!feof(fp)){
			cantidad_gates = vs1.cantidad_muestras/m_gate;
			muestras_sobrantes = vs1.cantidad_muestras%m_gate;

			/*----------------------------Canal V---------------------------*/
			for (int i=0;i<cantidad_gates;i++){ //muestras en polaridad Vertial
				sum_gate=0;
				for (int j=0; j<m_gate; j++){
					fread(&vs1.a,sizeof(float),1,fp); /* Se obtiene el valor de la polaridad en fase */
					fread(&vs1.b,sizeof(float),1,fp); /* Se obtiene el valor de la polaridad en cuadrante */
					sum_gate = sum_gate + sqrt(pow(vs1.a,2)+pow(vs1.b,2)); /* Se suma el valor absoluto de la compuerta */
				}
				matriz_v[i][pulso] = sum_gate/m_gate; /* Se llena la matriz */
			}

			fseek(fp,muestras_sobrantes*2*sizeof(float), SEEK_CUR); /* Se desplaza el puntero del fichero */ 

			/*----------------------------Canal H---------------------------*/

			for (int i=0;i<cantidad_gates;i++){ /* Se recorren las muestras en polaridad Horizontal */
				sum_gate=0;
				for (int j=0; j<m_gate; j++){
					fread(&vs1.a,sizeof(float),1,fp); /* Se obtiene el valor de la polaridad en fase */
					fread(&vs1.b,sizeof(float),1,fp); /* Se obtiene el valor de la polaridad en cuadrante */	
					sum_gate = sum_gate + sqrt(pow(vs1.a,2)+pow(vs1.b,2)); /* Se suma el valor absoluto de la compuerta */
				}

				matriz_h[i][pulso] = sum_gate/m_gate; /* Se llena la matriz */
				//printf("matriz_H[%d][%d]: %f\n", i,pulso,matriz_h[i][pulso]);
			}

			fseek(fp,muestras_sobrantes*2*sizeof(float), SEEK_CUR); /* Se desplaza el puntero del fichero */
			fread(&vs1.cantidad_muestras,2,1,fp); /* Se guarda la cantidad de muestras del proximo pulso */
			pulso ++;
		}
		fclose(fp);
	}
	return 0;
}

/*========================================================================================*/