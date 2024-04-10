#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>

void obtenerUtilizacionCPU(int pipefd[2]) { // Obtiene el porcentaje de utilizacion del CPU y lo escribe en un pipe
    FILE *file = fopen("/proc/stat", "r"); // Abrir archivo /proc/stat para lectura
    if (file == NULL) { // Validar
        char errorMsg[] = "Error al abrir /proc/stat";
        write(pipefd[1], errorMsg, strlen(errorMsg));
        return;
    }

    // Para almacenar valores de utilizacion del CPU
    char line[256]; 
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal; // Variables para los valores de utilizacion del CPU
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

    fclose(file); // Cerrar /proc/stat
}

void obtenerUtilizacionCPUProceso(int pipefd[2], char* pid) {
    char path[256];
    sprintf(path, "/proc/%s/stat", pid); // Construir ruta al archivo de estadisticas del proceso en /proc
    FILE* file = fopen(path, "r");
    if (file == NULL) { // Validacion
        char errorMsg[256];
        sprintf(errorMsg, "Error al abrir %s para lectura", path);
        write(pipefd[1], errorMsg, strlen(errorMsg));
        return;
    }

    unsigned long long int utime, stime; // Almacenar tiempo de CPU utilizado 
    fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %llu %llu", &utime, &stime); // Leer los datos
    fclose(file);

    // Obtener el numero total de ticks del proceso
    unsigned long long totalTicks = utime + stime;

    // Calcular el total de ticks de CPU disponibles en los ultimos 5 minutos
    long ticksPorSeg = sysconf(_SC_CLK_TCK);
    unsigned long long totalTicksDisp = ticksPorSeg * 60 * 5 * get_nprocs_conf();

    // Calcular el porcentaje de utilizacion del proceso
    double cpuUso = (double)totalTicks / totalTicksDisp * 100.0;

    // Preparar el mensaje a escribir en el pipe con la inforamcion recolectada
    char buffer[256];
    sprintf(buffer, "PID: %s, Uso aproximado de CPU en los ultimos 5 minutos: %.2f%%", pid, cpuUso);
    write(pipefd[1], buffer, strlen(buffer) + 1);

}





