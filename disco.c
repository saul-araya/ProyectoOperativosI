#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

double valores[3];

void porcentajeUsoMiB();
void porcentajeLibreMib();
void porcentajeUsoGiB();
void porcentajeLibreGiB();

int main(int agrc, char* argv[]){
    double porcentaje;
    int pipe = atoi(argv[2]);
    int opc = atoi(argv[1]);

    switch(opc){
        case 1:
            porcentajeUsoMiB();
            write(pipe, &valores, sizeof(porcentaje) * 3);
            break;
        case 2:
            porcentajeUsoGiB();
            write(pipe, &valores, sizeof(porcentaje) * 3);
            break;
        case 3:
            porcentajeLibreMib();
            write(pipe, &valores, sizeof(porcentaje) * 3);
            break;
        case 4:
            porcentajeLibreGiB();
            write(pipe, &valores, sizeof(porcentaje) * 3);
            break;
        default:
            break;
    }
    return 0;
}

void porcentajeUsoMiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double tamanoTotal = 0.0, tamanoUsado = 0.0, porcentaje=0.0;

    archivo = popen("df -BM /mnt/c", "r");  //Aqui se ejecuta el comando df -MB para ver las cantidades de uso del disco y se le pega la ruta del disco que se va a examinar en mi caso el c porque es el unico que tengo en esta pc
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){     //Aqui se hace este ciclo porque como si uno corre el comando le van a salir dos lineas o mejor dicho filas, la primera es donde viene como a que pertenece cada numero y hasta la segunda ya vienen los numeros, entonces use el ciclo para poder saltarme esos titulos y leer la linea de los numeros
        fgets(linea, 1024, archivo);
    }

    pclose(archivo);
    sscanf(linea, "%*s %lf %*s %lf", &tamanoTotal, &tamanoUsado);

    porcentaje = 100 * (tamanoUsado / tamanoTotal); 

    valores[0] = porcentaje;
    valores[1] = tamanoUsado;
    valores[2] = tamanoTotal;
}
void porcentajeLibreMib(){

    //Este metodo es lo mismo que el otro practicamente, solo que aqui en lugar de leer del comando el valor de cantidad usada se va a leer el de cantidad libre y ya

    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double tamanoTotal = 0.0, tamanoLibre = 0.0, porcentaje=0.0;

    archivo = popen("df -BM /mnt/c", "r");  
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);
    sscanf(linea, "%*s %lf %*s %*s %lf", &tamanoTotal, &tamanoLibre);

    porcentaje = 100 * (tamanoLibre / tamanoTotal); 

    valores[0] = porcentaje;
    valores[1] = tamanoLibre;
    valores[2] = tamanoTotal;
}
void porcentajeUsoGiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double tamanoUsado=0.0, tamanoTotal=0.0, porcentaje=0.0;
    
    archivo = popen("df -BG /mnt/c", "r");
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);

    sscanf(linea, "%*s %lf %*s %lf", &tamanoTotal, &tamanoUsado);

    porcentaje = 100 * (tamanoUsado / tamanoTotal);
    
    valores[0] = porcentaje;
    valores[1] = tamanoUsado;
    valores[2] = tamanoTotal;
}

void porcentajeLibreGiB(){
    FILE* archivo;
    char comandoDF[50];
    char linea[1024];
    double total = 0.0, cantidadLibre=0.0, porcentaje=0.0;
    
    archivo = popen("df -BG /mnt/c", "r");
    if(archivo == NULL){
        printf("\nError\n");
    }

    for(int i = 0; i < 2; i++){
        fgets(linea, 1024, archivo);     
    }

    pclose(archivo);

    sscanf(linea, "%*s %lf %*s %*s %lf", &total, &cantidadLibre);

    porcentaje = 100*(cantidadLibre / total);

    valores[0] = porcentaje;
    valores[1] = cantidadLibre;
    valores[2] = total;
}