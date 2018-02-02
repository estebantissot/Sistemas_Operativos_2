#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAM 512

void hostname();
void info_cpu();
void info_ram();
void info_kernel();

int main(){

	printf("Content-type: text/html\n\n");
	printf("<html>");
	

	printf("<TITLE>");
	printf("INFORMACION DE HARDWARE\n");
	printf("</TITLE>");
	fflush(0);

	printf("<HEAD>");
	printf("<CENTER>");
	printf("<h1>");
	hostname();
	printf("</h1>");
	printf("</CENTER>");
	printf("</HEAD>");
	fflush(0);

	printf("<body>");
	
	printf("<p><h3><li> UPTIME </li></h3></p>");
	fflush(0);
	system("uptime");
	fflush(0);

	printf("<p><h3><li> FECHA </li></h3></p>");
	fflush(0);
	system("date -R");

	printf("<p><h3><li> CPU </li></h3></p>");
	fflush(0);
	info_cpu();	

	printf("<p><h3><li> RAM </li></h3></p>");
	fflush(0);
	info_ram();	


	printf("<p><h3><li> KERNEL </li></h3></p>");
	fflush(0);
	info_kernel();


	printf("</body>");
	printf("</html>");
	
	return 0;
}

void hostname(){
	FILE *fhostname; 
	char buffer[TAM+1]; 

	fhostname = fopen("/proc/sys/kernel/hostname","r"); 
	fgets(buffer, TAM+1, fhostname); 
	fclose(fhostname);
	printf("%s",buffer);
	fflush(0); 
}


void info_kernel(){
	FILE * fversion;
	char cadena1[10], cadena2[10], cadena3[10];

	fversion=fopen("/proc/version","r");
	fscanf(fversion,"%s %s %s", cadena1,cadena2,cadena3);
	fclose(fversion); 

	printf("<p>%s %s %s</p>", cadena1,cadena2,cadena3);
	fflush(0);
 
}

void info_cpu(){
	FILE * fcpuinfo;
	char buffer[TAM];
	fcpuinfo=fopen("/proc/cpuinfo","r");
	
	fgets(buffer,TAM+1,fcpuinfo);
	while (!feof(fcpuinfo)) {
		if (!strncmp("model name",buffer,10)){
			printf("<p>%s</p>", buffer);
			fflush(0);
		}
		if (!strncmp("cache size",buffer,10)){
			printf("<p>%s</p>", buffer);
			fflush(0);
		}
		if (!strncmp("flags",buffer,5)){
			printf("<p>%s</p>", buffer);
			fflush(0);
			break;
		}
		fgets(buffer,TAM+1,fcpuinfo);
	}
	fclose(fcpuinfo);
}

void info_ram(){
	float memtotal, memfree, memdisp, buffers, cached;
	FILE *fmeminfo;

	fmeminfo=fopen("/proc/meminfo","r");
	fscanf(fmeminfo,"MemTotal: %f kB ",&memtotal);
	fscanf(fmeminfo,"MemFree: %f kB",&memfree);
	fscanf(fmeminfo,"MemAvailable: %f kB",&memdisp);
	fscanf(fmeminfo,"Buffers: %f kB",&buffers);
	fscanf(fmeminfo,"Cached: %f kB",&cached);
	fclose(fmeminfo);

	printf("<p>Total: %.2f MB</p>", memtotal/1024);
	printf("<p>Used: %.2f MB</p>", (memtotal-memfree-buffers-cached)/1024); 
	printf("<p>Free: %.2f MB</p>", memfree/1024);
	printf("<p>Available: %.2f MB</p>", memdisp/1024);
	fflush(0);
}
