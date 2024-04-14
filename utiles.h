struct StrategyRAM{
    double (*algoritmo)(long, long, long, double, double);
};

typedef struct StrategyRAM StrategyRAM;

double memoriaReal(long rss, long tamPag, long swap_pid, double ram, double swap){
    return ((double) ((rss * tamPag)/(1024*1024)) / ram)*100;
    //psUtilizacion = ((double) ((rss * tamPag)/(1024*1024)) / memoriaTotal)*100;
}

double memoriaRealPID(long rss, long tamPag, double ram){
    return ((double) ((rss * tamPag)/(1024*1024)) / ram)*100;
}


double memoriaVirtual(long rss, long tamPag, long swap_pid, double ram, double swap){
    return ((double) (swap_pid / 1024) / (swap)) * 100;
}


double memoriaVirtualPID(unsigned long swap_pid, double ram, double swap){
    return ((double) (swap_pid / 1024) / swap) * 100;
}
