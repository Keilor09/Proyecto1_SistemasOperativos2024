#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Función para obtener el porcentaje total de utilización del disco
void obtenerUtilizacionDiscoGiB(int pipe_fd[]) {
    // Cerrar el extremo de lectura del pipe en el proceso hijo
    close(pipe_fd[0]);

    // Comando para obtener las estadísticas de disco
    char comando[] = "df /"; // El comando df / muestra el tamaño en forma GiB
    // Buffer para almacenar la salida del comando
    char buffer[1024];

    // Ejecutar el comando y almacenar la salida en el buffer
    FILE* fp = popen(comando, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al ejecutar el comando para obtener las estadísticas de disco\n");
        return;
    }
    fgets(buffer, sizeof(buffer), fp); // Descartar la primera línea (encabezado)
    fgets(buffer, sizeof(buffer), fp); // Leer la segunda línea (datos de disco)
    pclose(fp); // Cerrar el stream del comando

    // Obtener el porcentaje total de utilización del disco
    char* porcentajeStr = strtok(buffer, " ");
    // Saltar columnas hasta llegar a la del porcentaje
    porcentajeStr = strtok(NULL, " "); 
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    float porcentajeUtilizacion = atof(porcentajeStr); // Convertir a flotante

    // Escribir las estadísticas en el pipe
    char output[1024]; // Buffer para almacenar el mensaje de salida
    sprintf(output, "(GiB) Porcentaje de utilización del disco: %f%%\n", porcentajeUtilizacion);
    write(pipe_fd[1], output, strlen(output));

    // Cerrar el extremo de escritura del pipe
    close(pipe_fd[1]);
}

// Función para obtener el porcentaje total de utilización del disco
void obtenerUtilizacionDiscoMiB(int pipe_fd[]) {
    // Cerrar el extremo de lectura del pipe en el proceso hijo
    close(pipe_fd[0]);

    // Comando para obtener las estadísticas de disco
    char comando[] = "df -B 1M /"; // El comando df / muestra el tamaño en forma MiB
    // Buffer para almacenar la salida del comando
    char buffer[1024];

    // Ejecutar el comando y almacenar la salida en el buffer
    FILE* fp = popen(comando, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al ejecutar el comando para obtener las estadísticas de disco\n");
        return;
    }
    fgets(buffer, sizeof(buffer), fp); // Descartar la primera línea (encabezado)
    fgets(buffer, sizeof(buffer), fp); // Leer la segunda línea (datos de disco)
    pclose(fp); // Cerrar el stream del comando

    // Obtener el porcentaje total de utilización del disco
    char* porcentajeStr = strtok(buffer, " ");
    // Saltar columnas hasta llegar a la del porcentaje
    porcentajeStr = strtok(NULL, " "); 
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    float porcentajeUtilizacion = atof(porcentajeStr); // Convertir a flotante

    // Escribir las estadísticas en el pipe
    char output[1024]; // Buffer para almacenar el mensaje de salida
    sprintf(output, "(MiB) Porcentaje de utilización del disco: %f%%\n", porcentajeUtilizacion);
    write(pipe_fd[1], output, strlen(output));

    // Cerrar el extremo de escritura del pipe
    close(pipe_fd[1]);
}

void obtenerLibreDisco(int pipe_fd[]) {
    // Cerrar el extremo de lectura del pipe en el proceso hijo
    close(pipe_fd[0]);

    // Comando para obtener las estadísticas de disco
    char comando[] = "df -h /"; // El comando df -h muestra el espacio de disco en formato legible por humanos
    // Buffer para almacenar la salida del comando
    char buffer[1024];

    // Ejecutar el comando y almacenar la salida en el buffer
    FILE* fp = popen(comando, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al ejecutar el comando para obtener las estadísticas de disco\n");
        return;
    }
    fgets(buffer, sizeof(buffer), fp); // Descartar la primera línea (encabezado)
    fgets(buffer, sizeof(buffer), fp); // Leer la segunda línea (datos de disco)
    pclose(fp); // Cerrar el stream del comando

    // Obtener el porcentaje total de utilización del disco
    char* porcentajeStr = strtok(buffer, " ");
    // Saltar columnas hasta llegar a la del porcentaje
    porcentajeStr = strtok(NULL, " "); 
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    porcentajeStr = strtok(NULL, " ");
    float porcentajeLibre = 100 - atof(porcentajeStr); // Se busca el total restando 100% al % que esta ocupado

    // Escribir las estadísticas en el pipe
    char output[1024]; // Buffer para almacenar el mensaje de salida
    sprintf(output, "Porcentaje libre del disco: %f%%\n", porcentajeLibre);
    write(pipe_fd[1], output, strlen(output));

    // Cerrar el extremo de escritura del pipe
    close(pipe_fd[1]);
}