#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void obtenerUtilizacionCPU(int pipefd[2]) {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL) {
        char error_msg[] = "Error al abrir /proc/stat";
        write(pipefd[1], error_msg, strlen(error_msg));
        return;
    }

    char line[256];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
            unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            unsigned long long cpu_usage = user + nice + system + irq + softirq + steal;
            double cpu_percentage = (double)cpu_usage / total * 100;
            char buffer[128];
            sprintf(buffer, "Porcentaje de utilización de CPU: %.2f%%", cpu_percentage);
            write(pipefd[1], buffer, strlen(buffer));
            break;
        }
    }

    fclose(file);
}



