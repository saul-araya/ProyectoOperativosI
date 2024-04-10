#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

unsigned long long idle;

long long tiempo_cpu();
double percent_total_cpu_ussage();
double percent_cpu_ussage_pid(int pid);

int main(int argc, char*argv[]){
    int pid = atoi(argv[1]);
    int pipe = atoi(argv[2]);
    double porcentaje = 0.0;

    if(pid > 0){
        porcentaje = percent_cpu_ussage_pid(pid);
        write(pipe, &porcentaje, sizeof(porcentaje));
    }
    else{
        porcentaje = percent_total_cpu_ussage();
        write(pipe, &porcentaje, sizeof(porcentaje));
    }
    return 0;
}

double percent_cpu_ussage_pid(int pid){
    char ruta[64];
    FILE *archivo;
    unsigned long utime, stime, cutime, cstime;
    double porcentaje;

    sprintf(ruta, "/proc/%d/stat", pid);  //Lo que va a hacer esta funcion es que va a poner el valor del pid que se ingreso en el medio de la ruta para poder accesar a la informacion de dicho proceso

    archivo = fopen(ruta, "r");
    if(archivo==NULL){
        printf("ERROR");
    }

    fscanf(archivo, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu %lu %lu", 
           &utime, &stime, &cutime, &cstime);
    
    fclose(archivo);

    long tiempo_uso_pid = utime + stime + cutime + cstime;

    long long tiempoTotal = tiempo_cpu();

    return (double)100*((double)tiempo_uso_pid / (double)tiempoTotal);
}

double percent_total_cpu_ussage(){
    long long tiempoTotal = tiempo_cpu();
    return (double)(100.0*(((double)tiempoTotal - idle) / (double)tiempoTotal));
}

long long tiempo_cpu(){
    FILE *archivo;
    char linea[128];
    long long user, nice, system, iowait, irq, softirq, steal, guest, guest_nice;
    long long tiempoTotal;
    float porcentaje;

    archivo = fopen("/proc/stat", "r");
    if(archivo == NULL){
        printf("Error\n");
    } 
    
    fgets(linea, sizeof(linea), archivo);
    fclose(archivo);
    sscanf(linea, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
       &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    tiempoTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    return tiempoTotal; //se retorna el tiempo total de uso del cpu, desde que se inicio, contando el tiempo muerto del cpu*/
}