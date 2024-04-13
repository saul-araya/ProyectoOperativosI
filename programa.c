#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "utiles.h"
#include <signal.h>

int option(char* argument);
int discoParametro(char* argument);

//df disco
//free memoria

/* FUNCIONES DE LAS OPCIONES DE MONITOREO */
void cpu(int _pid);
void ram(int _pid);
void disc(int parametro);

/* IMPRESION DE LOS VALORES DEL MONITOR DE CPU */
void imprimirResultadoCpu(int vec[]);
void imprimirResultadoCpuPID(int vec[], int _pid);

/* IMPRESION DE LOS VALORES DEL MONITOR DE DISCO */
void imprimir_result_disc_uti_mib(int vec[]);
void imprimir_result_disc_lib_mib(int vec[]);
void imprimir_result_disc_uti_gib(int vec[]);
void imprimir_result_disc_lib_gib(int vec[]);

/* IMPRESION DE LOS VALORES DEL MONITOR RAM*/
void imprimirResultadoRAM(int vec[]);

void imprimirResultadoRamPID(int vec[], int pid);

void man();

pid_t ram_pid;

int main(int argc, char*argv[]){

    if(argc==1){
        man();
        return 0;
    }

    int opc = option(argv[1]);
    int pid = 0, discArg = 0;

    if(argc == 3 && opc == 1){
        pid = atoi(argv[2]);
    }
    else if(argc == 3 && opc == 3){
        discArg = discoParametro(argv[2]);
    }

    switch(option(argv[1])){
        case 1:
            cpu(pid);
            break;
        case 2:
            ram(pid);
            break;
        case 3:
            disc(discArg);
            break;
        default:
            printf("Parametros no validos\n");
            break;
    }
    return 0;
}

int option(char* argument){
    if(strcmp(argument, "cpu")==0){
        return 1;
    }
    else if(strcmp(argument, "ram")==0){
        return 2;
    }
    else if(strcmp(argument, "disc")==0){
        return 3;
    }
    else{
        return 0;
    }
}

int discoParametro(char* argument){
    if(strcmp(argument, "-tm")==0){
        return 1;
    }
    else if(strcmp(argument, "-tg")==0){
        return 2;
    }
    else if(strcmp(argument, "-lm")==0){
        return 3;
    }
    else if(strcmp(argument, "-lg")==0){
        return 4;
    }
    else{
        return 0;
    }
}

void cpu(int _pid){
    int vec[2];

    if(pipe(vec)){
        printf("Error al crear el pipe");
    }

    pid_t pid = fork();
    if(pid < 0){
        printf("Error\n");
    }
    if(pid==0){
        close(vec[0]);

        char pid_caracter[20];
        char pipe[20];
        sprintf(pid_caracter, "%d", _pid);
        sprintf(pipe, "%d", vec[1]);

        char* arguments[] = {"cpu", pid_caracter, pipe, NULL};
        execv("/usr/local/bin/cpu", arguments);
    }
    else{
        wait(NULL);
        close(vec[1]);
        if(_pid <= 0){
            imprimirResultadoCpu(vec);
        }
        else{
            imprimirResultadoCpuPID(vec, _pid);
        }
    }
}
void ram(int _pid){
    int vec[2];

    if(pipe(vec))
    {
        printf("Error al crear la tubería en el monitor de memoria RAM");
        exit(1);
    }

    ram_pid = fork();
    if(ram_pid < 0){
        printf("Error\n");
    }
    if(ram_pid==0){
        close(vec[0]);

        char pid_caracter[20];
        char pipe[20];

        sprintf(pid_caracter, "%d", _pid);
        sprintf(pipe, "%d", vec[1]);

        char* arguments[] = {"ram", pid_caracter, pipe, NULL};

        execv("/usr/local/bin/ram", arguments);

    }
    else{
        wait(NULL);
        close(vec[1]);

        imprimirResultadoRAM(vec);

        printf("padre: Hijo RAM finalizo\n");
    }
}
void disc(int parametro){
    int vec[2];

    if(pipe(vec)){
        printf("Error al crear el pipe");
    }

    pid_t pid = fork();
    if(pid < 0){
        printf("Error\n");
    }
    if(pid==0){
        close(vec[0]);

        char pipe[20];
        char argumento[20];
        sprintf(pipe, "%d", vec[1]);
        sprintf(argumento, "%d", parametro);

        char* arguments[] = {"disco", argumento, pipe, NULL};
        execv("/usr/local/bin/disco", arguments);
    }
    else{
        double porcentaje = 0.0;
        wait(NULL);
        switch(parametro){
            case 1:
                imprimir_result_disc_uti_mib(vec);
                break;
            case 2:
                imprimir_result_disc_uti_gib(vec);
            break;
            case 3:
                imprimir_result_disc_lib_mib(vec);
            break;
            case 4:
                imprimir_result_disc_lib_gib(vec);
            break;
        }
    }
}

void imprimirResultadoCpu(int vec[]){
    double porcentaje = 0.0;
    close(vec[1]);
    read(vec[0], &porcentaje, sizeof(porcentaje));
    printf("El porcentaje de uso del CPU es de: %.1lf%%\n", porcentaje);
}
void imprimirResultadoCpuPID(int vec[], int _pid){
    double porcentaje = 0.0;
    char nombre[1024];
    close(vec[1]);
    read(vec[0], &porcentaje, sizeof(porcentaje));
    read(vec[0], &nombre, sizeof(nombre));
    printf("Nombre del proceso: %s\n", nombre);
    printf("PID: %d\n", _pid);
    printf("Porcentaje de uso del CPU de este proceso en los ultimos 5 minutos es de: %.1f%%\n", porcentaje);
}
void imprimir_result_disc_uti_mib(int vec[]){
    double valores[3];
    close(vec[1]);
    read(vec[0], &valores, sizeof(valores));
    printf("Tamano en MiB de la cantidad usada del disco: %2.fM\n", valores[1]);
    printf("Tamano en MiB del espacio total del disco: %2.fM\n", valores[2]);
    printf("Porcentaje de utilizacion total del disco en MiB es de: %2.f%%\n", valores[0]);
}
void imprimir_result_disc_lib_mib(int vec[]){
    double valores[3];
    close(vec[1]);
    read(vec[0], &valores, sizeof(valores));
    printf("Tamano en MiB de la cantidad libre del disco: %2.fM\n", valores[1]);
    printf("Tamano en MiB del espacio total del disco: %2.fM\n", valores[2]);
    printf("Porcentaje libre del disco en MiB es de: %2.f%%\n", valores[0]);
}
void imprimir_result_disc_uti_gib(int vec[]){
    double valores[3];
    close(vec[1]);
    read(vec[0], &valores, sizeof(valores));
    printf("Tamano en GiB de la cantidad usada del disco: %2.fG\n", valores[1]);
    printf("Tamano en GiB del espacio total del disco: %2.fG\n", valores[2]);
    printf("Porcentaje de utilizacion del disco en GiB es de: %2.f%%\n", valores[0]);
}
void imprimir_result_disc_lib_gib(int vec[]){
    double valores[3];
    close(vec[1]);
    read(vec[0], &valores, sizeof(valores));
    printf("Tamano en GiB de la cantidad libre del disco: %2.fG\n", valores[1]);
    printf("Tamano en GiB del espacio total del disco: %2.fG\n", valores[2]);
    printf("Porcentaje libre del disco en GiB es de: %2.f%%\n", valores[0]);
}

void imprimirResultadoRAM(int vec[]){
    close(vec[1]);
    double porcentajeF = 0.0;

    proceso_info * aux = (proceso_info*) malloc(sizeof(proceso_info));
    
    printf("|%-10s %-40s %-10s|\n", "PID", "Nombre del proceso", "MEM Fisica");
    while(read(vec[0], aux, sizeof(proceso_info)) > 0){
        printf("|%-10s %-40s %-10lf|\n", aux->pid, aux->nombre, aux->porcentaje);
        porcentajeF += aux->porcentaje;
    }

    if(read <= 0){
        printf("\nNo hay procesos que leer\n");
    }

    printf("\nPorcentaje de uso total de la memoria fìsica: %lf\n", porcentajeF);
    free(aux);

}


void man(){
    printf("---------------------------How to Use---------------------------\n");
    printf("CPU:\n");
    printf("\tTotal CPU usage percentage: \t./programa cpu\n");
    printf("\tUsage percentage per process: \t./programa cpu [PID]\n");
    printf("RAM:\n");
    printf("\tRAM:\n");
    printf("\tRAM:\n");
    printf("Disk:\n");
    printf("\tDisk usage percentage in MiB: \t./programa disc -tm\n");
    printf("\tDisk usage percentage in GiB: \t./programa disc -tg\n");
    printf("\tDisk free percentage in MiB: \t./programa disc -lm\n");
    printf("\tDisk free percentage in GiB: \t./programa disc -lg\n");
}