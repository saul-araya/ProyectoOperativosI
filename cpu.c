#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

double total_cpu_ussage();
void cpu_ussage_pid(int pid);

int main(int argc, char*argv[]){
    int pid = atoi(argv[1]);
    int pipe = atoi(argv[2]);

    if(pid > 0){
        int x = (int)getpid();
        cpu_ussage_pid(x);
    }
    else{
        double porcentaje = total_cpu_ussage();
        write(pipe, &porcentaje, sizeof(porcentaje));
    }
    return 0;
}

void cpu_ussage_pid(int pid){
    char ruta[64];
    FILE *archivo;
    unsigned long long tiempoUsuario, tiempoSistema, tiempoFinal;
    double porcentaje;

    sprintf(ruta, "/proc/%d/stat", pid);

    archivo = fopen(ruta, "r");
    if(archivo==NULL){
        printf("ERROR");
    }

    fscanf(archivo, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %llu %llu", &tiempoUsuario, &tiempoSistema);

    fclose(archivo);

    tiempoFinal = tiempoUsuario + tiempoSistema;
    printf("tiempo: %lld", tiempoFinal);
    porcentaje = (double)100*(tiempoFinal/sysconf(_SC_CLK_TCK));
    printf("porcentaje: %f\n", porcentaje);
}

double total_cpu_ussage(){
    FILE *archivo;
    char linea[128];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long long tiempoTotal;
    double porcentaje;

    archivo = fopen("/proc/stat", "r");
    if(archivo == NULL){
        printf("Error\n");
    }

    fgets(linea, sizeof(linea), archivo);
    fclose(archivo);
    sscanf(linea, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
       &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    tiempoTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    porcentaje = (double)100*(double)((double)(tiempoTotal - idle) / tiempoTotal);
    return porcentaje;
}