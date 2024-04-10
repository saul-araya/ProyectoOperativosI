#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int option(char* argument);

void cpu(int _pid);
void ram(int _pid);
void disc();

void imprimirResultadoCpu(int vec[]);
void imprimirResultadoCpuPID(int vec[], int _pid);

int main(int argc, char*argv[]){

    int pid = 0;
    if(argc == 3){
        pid = atoi(argv[2]);
    }

    switch(option(argv[1])){
        case 1:
            printf("CPU\n");
            cpu(pid);
            break;
        case 2:
            printf("RAM\n");
            ram(pid);
            break;
        case 3:
            printf("DISC\n");
            disc();
            break;
        default:
            break;
    }
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
        execv("./cpu", arguments);
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
    pid_t pid = fork();
    if(pid < 0){
        printf("Error\n");
    }
    if(pid==0){
        printf("HIJO\n");
    }
    else{
        wait(NULL);
        printf("padre\n");
    }
}
void disc(){
    pid_t pid = fork();
    if(pid < 0){
        printf("Error\n");
    }
    if(pid==0){
        printf("HIJO\n");
    }
    else{
        wait(NULL);
        printf("padre\n");
    }
}

void imprimirResultadoCpu(int vec[]){
    double porcentaje = 0.0;
    close(vec[1]);
    read(vec[0], &porcentaje, sizeof(porcentaje));
    printf("El porcentaje de uso del CPU es de: %f\n", porcentaje);
}
void imprimirResultadoCpuPID(int vec[], int _pid){
    double porcentaje = 0.0;
    close(vec[1]);
    read(vec[0], &porcentaje, sizeof(porcentaje));
    printf("El porcentaje de uso del CPU del proceso con PID: %d es de: %f\n", _pid, porcentaje);
}