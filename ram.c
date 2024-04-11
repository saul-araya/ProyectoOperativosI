#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <dirent.h>

int filtrarCarpetaPID(char*);
void memoriaRealTotal();
double memoriaRAMMB();

int main(int argc, char* argv[]){
    memoriaRealTotal();

    printf("Memoria RAM total física en el sistema [En MB=%f]\n", memoriaRAMMB());

    return 0;
}

void memoriaRealTotal(){
    FILE* archivo;
    DIR *directorio_proc;

    double memoriaTotal = memoriaRAMMB();

    double porcentaje_virtual = 0.0;
    double psUtilizacion = 0.0;
    long tamPag = sysconf(_SC_PAGESIZE);


    char* pid;
    char ruta[256];
    char nombreProceso[100];
    unsigned long vmsize;
    long rss; 

   

    

    struct dirent* fichero;

    directorio_proc = opendir("/proc/");

    if(directorio_proc == NULL){
        printf("Fallo en abrir el archivo proc (Monitor de memoria).\n");
        exit(1);
    }

    while((fichero = readdir(directorio_proc)) != NULL){
        if(fichero->d_type == 4){
            if(!filtrarCarpetaPID(fichero->d_name)){
                pid = fichero->d_name;
                sprintf(ruta, "/proc/%s/stat", pid);
                
                archivo = fopen(ruta, "r");
                fscanf(archivo, "%*d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*llu %lu %ld",
                    nombreProceso, &vmsize, &rss);
                fclose(archivo);

                psUtilizacion = (double) ((rss * tamPag)/(1024*1024)) / memoriaTotal;

                printf("____________Tamaño de la página del sistema: %ld________________\n\n", tamPag);
                printf("PID: %s\nNombre: %s\nRSS: %ld\nPorcentaje: %f\n\n", pid, nombreProceso, rss, psUtilizacion);
                
            }
        }
    }

    return;
}

int filtrarCarpetaPID(char* proc_fichero){
    char* expresion = "^[0-9]+$";
    regex_t expresion_regex;


    int compilacion_regex = regcomp(&expresion_regex, expresion, REG_EXTENDED);
    if(compilacion_regex){
            printf("Error al compilar la expresión regular en el monitor de memoria...\n");
            return -1;
    }

    int res = regexec(&expresion_regex, proc_fichero, 0, NULL, 0);

    regfree(&expresion_regex); 
    
    return res;
}

double memoriaRAMMB(){
    double memoriaRam = 0.00;

    long paginasFisicas = sysconf(_SC_PHYS_PAGES); 
    long tamPag = sysconf(_SC_PAGESIZE);

    memoriaRam = (double) (tamPag * paginasFisicas) / (1024 * 1024);


    return memoriaRam;
}