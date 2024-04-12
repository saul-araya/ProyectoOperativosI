#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

unsigned long long idle;

double tiempo_cpu();
void nombreProceso(int _pid, char* nombre);
long tiempoTotalCpu();
double percent_cpu_ussage_pid(int pid);

int main(int argc, char*argv[]){
    int pid = atoi(argv[1]);
    int pipe = atoi(argv[2]);
    double porcentaje = 0.0;
    char nombre[1024];

    if(pid > 0){
        porcentaje = percent_cpu_ussage_pid(pid);
        nombreProceso(pid, nombre);
        write(pipe, &porcentaje, sizeof(porcentaje));
        write(pipe, &nombre, sizeof(nombre));
    }
    else{
        porcentaje = tiempo_cpu();
        write(pipe, &porcentaje, sizeof(porcentaje));
    }
    return 0;
}
long tiempoTotalCpu(){
    FILE *archivo;
    char linea[128];
    long ticksPorSegundoCpu = sysconf(_SC_CLK_TCK);
    unsigned long usuario, nice, sistema, idle, steal, guest, guest_nice;

    archivo = fopen("/proc/stat", "r");
    if(archivo==NULL){
        printf("ERROR");
    }
    
    fscanf(archivo, "cpu %lu %lu %lu %lu %*u %*u %*u %lu %lu %lu", &usuario, &nice, &sistema, &idle, &steal, &guest, &guest_nice);
    
    return usuario + nice + sistema + idle + steal + guest + guest_nice;
}
double percent_cpu_ussage_pid(int pid){
    char ruta[64];
    FILE *archivo;

    unsigned long utime, stime, jiffiesTotales = 0.0;;
    long ticksPorSegundoCpu = sysconf(_SC_CLK_TCK); //Esto me devuelve el valor jiffies del CPU por segundo, me va a servir para poder hacer la conversion de jiffies a segundos

    double segundosProceso = 0.0, porcentaje = 0.0;

    sprintf(ruta, "/proc/%d/stat", pid);  //Lo que va a hacer esta funcion es que va a poner el valor del pid que se ingreso en el medio de la ruta para poder accesar a la informacion de dicho proceso

    archivo = fopen(ruta, "r");
    if(archivo==NULL){
        printf("ERROR");
    }

    fscanf(archivo, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", 
           &utime, &stime);
    
    fclose(archivo);

    jiffiesTotales = utime + stime;
    segundosProceso = (double)(jiffiesTotales) / ticksPorSegundoCpu;

    segundosProceso = segundosProceso * 300.00; //Estos 300 son equivalente a los 5 minutos (es una estimacion ya que en si el tiempo de uso de cpu puede ir variando en poco tiempo)

    long tiempoTotalCpuJ = tiempoTotalCpu();

    double tiempoTotalCpuSegundos = (double)tiempoTotalCpuJ / ticksPorSegundoCpu;   //Se hace el mismo procedimiento para calcular ahora el tiempo de uso total del cpu en segundos para poder calcular el procentaje.

    porcentaje = 100*(segundosProceso / tiempoTotalCpuSegundos);

    return porcentaje;
}
double tiempo_cpu(){
    FILE *archivo;
    char linea[128];
    long long user, nice, system, iowait, irq, softirq, steal, guest, guest_nice;
    long long tiempoTotal;
    double usuario=0.0, sistema=0.0;

    archivo = popen("top -b -n 1 | grep 'Cpu(s)'", "r");
    if(archivo == NULL){
        printf("Error\n");
    }

    fgets(linea, sizeof(linea), archivo);
    sscanf(linea, "%*s %lf %*s %lf", &usuario, &sistema);

    double porcentaje = usuario + sistema;
    return porcentaje;
}
void nombreProceso(int _pid, char* nombre){
    FILE *archivo;
    char linea[128];
    char ruta[64];
    
    sprintf(ruta, "/proc/%d/stat", _pid);  //Lo que va a hacer esta funcion es que va a poner el valor del pid que se ingreso en el medio de la ruta para poder accesar a la informacion de dicho proceso

    archivo = fopen(ruta, "r");
    if(archivo==NULL){
        printf("ERROR");
    }
    fgets(linea, sizeof(linea), archivo);

    fclose(archivo);

    sscanf(linea, "%*d (%[^)])", nombre);
}