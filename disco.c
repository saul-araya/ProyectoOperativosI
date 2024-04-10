#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

double porcentajeUsoMiB();
double porcentajeLibreMib();
double porcentajeUsoGiB();
double porcentajeLibreGiB();

int main(int agrc, char* argv[]){
    double porcentaje = 0.0;
    int pipe = atoi(argv[2]);
    int opc = atoi(argv[1]);

    switch(opc){
        case 1:
            porcentaje = porcentajeUsoMiB();
            write(pipe, &porcentaje, sizeof(porcentaje));
            break;
        case 2:
            porcentaje = porcentajeUsoGiB();
            write(pipe, &porcentaje, sizeof(porcentaje));
            break;
        case 3:
            porcentaje = porcentajeLibreMib();
            write(pipe, &porcentaje, sizeof(porcentaje));
            break;
        case 4:
            porcentaje = porcentajeLibreGiB();
            write(pipe, &porcentaje, sizeof(porcentaje));
            break;
        default:
            break;
    }
    return 0;
}

double porcentajeUsoMiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double tamanoTotal = 0.0, tamanoUsado = 0.0;

    archivo = popen("df -BM /mnt/c", "r");  //Aqui se ejecuta el comando df -MB para ver las cantidades de uso del disco y se le pega la ruta del disco que se va a examinar en mi caso el c porque es el unico que tengo en esta pc
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){     //Aqui se hace este ciclo porque como si uno corre el comando le van a salir dos lineas o mejor dicho filas, la primera es donde viene como a que pertenece cada numero y hasta la segunda ya vienen los numeros, entonces use el ciclo para poder saltarme esos titulos y leer la linea de los numeros
        fgets(linea, 1024, archivo);
    }

    pclose(archivo);
    sscanf(linea, "%*s %lf %*s %lf", &tamanoTotal, &tamanoUsado);
    return 100 * (tamanoUsado / tamanoTotal);
}
double porcentajeLibreMib(){

    //Este metodo es lo mismo que el otro practicamente, solo que aqui en lugar de leer del comando el valor de cantidad usada se va a leer el de cantidad libre y ya

    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double tamanoTotal = 0.0, tamanoLibre = 0.0;

    archivo = popen("df -BM /mnt/c", "r");  
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);
    sscanf(linea, "%*s %lf %*s %*s %lf", &tamanoTotal, &tamanoLibre);

    return 100 * (tamanoLibre / tamanoTotal);
}
double porcentajeUsoGiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double porcentaje=0.0;
    
    archivo = popen("df -BG /mnt/c", "r");
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);

    sscanf(linea, "%*s %*s %*s %*s %lf", &porcentaje);

    return porcentaje;
}

double porcentajeLibreGiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double total = 0.0, cantidadLibre=0.0;
    
    archivo = popen("df -BG /mnt/c", "r");
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);

    sscanf(linea, "%*s %lf %*s %*s %lf", &total, &cantidadLibre);

    return 100*(cantidadLibre / total);
}