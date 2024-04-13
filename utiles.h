struct proceso_info{
    char nombre[100];
    char pid[100];
    double porcentaje;

    struct proceso_info * sig;
};

typedef struct proceso_info proceso_info;