#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void obtenerUtilizacionCPU(int pipefd[2]) { // Obtiene el porcentaje de utilizacion del CPU y lo escribe en un pipe
    FILE *file = fopen("/proc/stat", "r"); // Abrir archivo /proc/stat para lectura
    if (file == NULL) { // Validar
        char errorMsg[] = "Error al abrir /proc/stat";
        write(pipefd[1], errorMsg, strlen(errorMsg));
        return;
    }

    // Para almacenar valores de utilizacion del CPU
    char line[256]; 
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    while (fgets(line, sizeof(line), file)) {
        // Buscar linea que comienza por cpu
        if (strncmp(line, "cpu ", 4) == 0) {
            // Analizar la línea para obtener los valores de utilización de la CPU
            sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
            // Calcular el total de tiempo de CPU
            unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            // Calcular el tiempo de CPU utilizado
            unsigned long long cpuUso = user + nice + system + irq + softirq + steal;
            // Calcular porcentaje de utilizacion de CPU
            double cpuPorcentaje = (double)cpuUso / total * 100;
            // Cadena de texto con el porcentaje de utilizacion del CPU
            char buffer[128];
            sprintf(buffer, "Porcentaje de utilizacion de CPU: %.2f%%", cpuPorcentaje);
            // Escribir mensaje en pipe
            write(pipefd[1], buffer, strlen(buffer));
            break;
        }
    }

    fclose(file);
}




