#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include "utiles.h"
#include <signal.h>

int filtrarCarpetaPID(char*);
proceso_info * memoriaRealTotal();
double memoriaRAMMB();

int main(int argc, char* argv[]){
    int pid = atoi(argv[1]);
    int pipe = atoi(argv[2]);



    proceso_info * resultado = memoriaRealTotal();
    proceso_info * actual = resultado;

    int i = 0;

    while(actual != NULL && i < 60){
        //printf("Leyendo datos\n");
        //printf("Datos de la lista: %s, %s, %lf\n",actual->pid ,actual->nombre, actual->porcentaje);
        write(pipe, actual, sizeof(proceso_info));
        actual = actual->sig;
        i++;
    }
    

    return 0;
}

proceso_info * memoriaRealTotal(){
    FILE* archivo;
    DIR *directorio_proc;

    double memoriaTotal = memoriaRAMMB();

    double porcentaje_virtual = 0.0;
    double psUtilizacion = 0.0;
    long tamPag = sysconf(_SC_PAGESIZE);


    char *pid;
    char nombreProceso[100];
    char ruta[256];
    unsigned long vmsize;
    long rss;
    //double porcentajeF = 0.0;

    struct dirent* fichero;
    proceso_info * ppio = NULL;
    proceso_info * actual = NULL;
    proceso_info * aux = NULL;
    

    directorio_proc = opendir("/proc/");

    if(directorio_proc == NULL){
        printf("Fallo en abrir el archivo proc (Monitor de memoria).\n");
        exit(1);
    }
    
    //printf("|%-10s %-40s %-10s|\n", "PID", "Nombre del proceso", "MEM Fisica");

    while((fichero = readdir(directorio_proc)) != NULL){
        if(fichero->d_type == 4){
            if(!filtrarCarpetaPID(fichero->d_name)){
                pid = fichero->d_name;
                sprintf(ruta, "/proc/%s/stat", pid);

                archivo = fopen(ruta, "r");
                fscanf(archivo, "%*d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*llu %lu %ld",
                    nombreProceso, &vmsize, &rss);
                fclose(archivo);

                psUtilizacion = ((double) ((rss * tamPag)/(1024*1024)) / memoriaTotal)*100;

                aux = (proceso_info *) malloc(sizeof(proceso_info));
                    strcpy(aux->nombre, nombreProceso);
                    strcpy(aux->pid, pid);
                    
                    aux->porcentaje = psUtilizacion;
                    aux->sig = NULL;
                
                if(ppio == NULL){
                    ppio = aux;
                }else{
                    actual->sig = aux; 
                }

                actual = aux;
                //porcentajeF += psUtilizacion;

                //printf("|%-10s %-40s %-10lf|\n", pid, nombreProceso, psUtilizacion);
                //printf("PID: %s\nNombre: %s\nRSS: %ld\nPorcentaje: %lf\n\n", pid, nombreProceso, rss, psUtilizacion*100);
                
            }
        }
    }
    //printf("\n::::::::::Porcentaje de uso total de la memoria fìsica: %lf\n", porcentajeF);

    return ppio;
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