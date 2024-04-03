#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funcion para obtener el porcentaje general de utilizacion de CPU de todos los procesos
void obtenerUtilizacionCPU(int pipe_fd[]) {
    // Cerrar el extremo de lectura del pipe en el proceso hijo
    close(pipe_fd[0]);
    
    // Comando para obtener las estadisticas de CPU de todos los procesos (utilizando el archivo /proc/stat)
    char comando[] = "cat /proc/stat | grep '^cpu '";
    // Buffer para almacenar la salida del comando
    char buffer[1024];
    // Ejecutar el comando y almacenar la salida en el buffer
    FILE* fp = popen(comando, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al ejecutar el comando para obtener las estadísticas de CPU\n");
        return;
    }
    fgets(buffer, sizeof(buffer), fp); // Leer una linea del archivo y almacenarla en el buffer
    pclose(fp); // Cerrar el stream del archivo

    int user, nice, system, idle, iowait, irq, softirq, steal;
    sscanf(buffer, "cpu %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal); // Extraer los valores de la linea
    
    int total = user + nice + system + idle + iowait + irq + softirq + steal; // Calcular el total de los tiempos del CPU
    float porcentaje_utilizacion = ((float)(total - idle) / total) * 100; // Calcular el porcentaje de utilizacion del CPU
    
    // Escribir las estadisticas en el pipe
    char output[1024]; // Buffer para almacenar el mensaje de salida
    sprintf(output, "Porcentaje de utilización de CPU de todos los procesos: %f%%\n", porcentaje_utilizacion);
    write(pipe_fd[1], output, strlen(output));
    
    close(pipe_fd[1]); // Cerrar el extremo de escritura del pipe
}
