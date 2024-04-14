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
void memoriaTotal(StrategyRAM strategia, char* tipo);
double memoriaRAMMB();
void handlerRam(int senal){}
double memoriaSwapMB();
void memoriaPID(StrategyRAM strategia, int pid, char* tipo, int pipe);

int main(int argc, char* argv[]){

    int pid = atoi(argv[1]);
    char* opcion = argv[2];
    int pipe = atoi(argv[3]);
    char proceso_info[260];

    StrategyRAM strategia;
    
    if(strcmp(opcion, "-v") == 0){
        strategia.algoritmo = &memoriaVirtual;
    }else{
        strategia.algoritmo = &memoriaReal;
    }   

    if(pid <= 0){
        if(strcmp(opcion, "-v") == 0){
            memoriaTotal(strategia, "Virtual");
        }else{
            
            memoriaTotal(strategia, "Real");
        }
        
    }else if(pid > 0){
        if(strcmp(opcion, "-v")){
            memoriaPID(strategia, pid, "Virtual", pipe);
        }else{
            memoriaPID(strategia, pid, "Real", pipe);
        }
    }else{
        printf("\nError: argumentos no validos");
    }
    
    return 0;
}

void memoriaTotal(StrategyRAM strategia, char* tipo){
    FILE* archivo;
    FILE* statusAr;
    DIR *directorio_proc;

    

    double porcentaje_virtual = 0.0;
    double psUtilizacion = 0.0;
    long tamPag = sysconf(_SC_PAGESIZE);


    char *pid;
    char nombreProceso[100];
    char ruta[256];
    char rutaStatus[256];
    long rss;
    double porcentajeF = 0.0;
    double memoriaSwap = memoriaSwapMB();
    double memoriaTotal = memoriaRAMMB();
    long swap_mem;
    char buffer[120];

    struct dirent* fichero; 

    char tipo_col[40];

    sprintf(tipo_col, "%% memoria %s", tipo);

    directorio_proc = opendir("/proc/");

    if(directorio_proc == NULL){
        printf("Fallo en abrir el archivo proc (Monitor de memoria).\n");
        exit(1);
    }
    
    printf("|%-10s %-40s %-22s|\n", "PID", "Nombre del proceso", tipo_col);

    while((fichero = readdir(directorio_proc)) != NULL){
        if(fichero->d_type == 4){
            if(!filtrarCarpetaPID(fichero->d_name)){
                pid = fichero->d_name;
                sprintf(ruta, "/proc/%s/stat", pid);
                sprintf(rutaStatus, "cat /proc/%s/status | grep 'VmSwap:'", pid);

                archivo = fopen(ruta, "r");
                fscanf(archivo, "%*d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*llu %*lu %ld",
                    nombreProceso, &rss);
                fclose(archivo);

                statusAr = popen(rutaStatus, "r");
                fgets(buffer, 120, statusAr);
                pclose(statusAr);

                sscanf(buffer, "%*s %ld", &swap_mem);


                psUtilizacion = strategia.algoritmo(rss, tamPag, swap_mem, memoriaTotal, memoriaSwap);
                porcentajeF += psUtilizacion;
                
                printf("|%-10s %-40s %10.2f%% %-10s|\n", pid, nombreProceso, psUtilizacion, " ");
                
            }
        }
    }
    printf("\n-- Porcentaje total de uso de memoria %s: %.2f%%\n", tipo, porcentajeF);
}

double memoriaSwapMB(){
    FILE * archivo;
    char buffer[1024];
    long swapTotal;
    archivo = popen("free | grep 'Swap:'", "r");

    fgets(buffer, 1024, archivo);

    pclose(archivo);

    sscanf(buffer, "%*s %ld", &swapTotal);

    return (double) swapTotal / 1024;

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

void memoriaPID(StrategyRAM strategia, int pid, char* tipo, int pipe){
    FILE* archivo;
    FILE* statusAr;

    char ruta[250];
    char rutaStatus[250];
    char nombreProceso[100];
    long rss;
    long tamPag = sysconf(_SC_PAGESIZE);
    long swap_mem;
    char buffer[200];
    char datos[300];
    char* ptr;

    double memoriaTotal = memoriaRAMMB();
    double memoriaSwap = memoriaSwapMB();


    double porcentaje = 0.0;

    sprintf(ruta, "/proc/%d/stat", pid);
    sprintf(rutaStatus, "cat /proc/%d/status | grep 'VmSwap:'", pid);

    archivo = fopen(ruta, "r");
    fscanf(archivo, "%*d (%[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*llu %*lu %ld",
                    nombreProceso, &rss);
    fclose(archivo);


    statusAr = popen(rutaStatus, "r");
    fgets(buffer, 120, statusAr);
    pclose(statusAr);

    sscanf(buffer, "%*s %ld", &swap_mem);

    porcentaje = strategia.algoritmo(rss, tamPag, swap_mem, memoriaTotal, memoriaSwap);

    sprintf(datos, "PID: %d, Nombre: %s, %% Mem %s: %.2f%%\n", pid, nombreProceso, tipo, porcentaje);

    write(pipe, datos, sizeof(datos));
    close(pipe);
}