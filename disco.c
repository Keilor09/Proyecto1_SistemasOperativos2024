#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void obtenerPorcentajeUsoDiscoEnMiB(int pipefd[2]) {
    FILE *fp; // Declarar puntero a FILE para manejar el proceso que ejecutara el comando
    char path[1035]; // Buffer para almacenar cada linea de salida del comando
    char buffer[256]; // Buffer para almacenar el mensaje a enviar por el pipe
    double porcentaje; // Almacenar porcentaje de uso del disco

    // Ejecutar el comando df -BM y capturar su salida en modo MiB
    fp = popen("df -m /", "r"); // Abrir proceso que ejecuta el comando en modo de lectura
    if (fp == NULL) { // Validacion
        perror("Error al ejecutar df -BM");
        exit(EXIT_FAILURE);
    }

    // Leer la salida línea por línea
    while (fgets(path, sizeof(path)-1, fp) != NULL) { // Leer cada linea de la salida del comando
        // Procesar la línea que contiene el espacio utilizado y disponible
        if (strstr(path, "/dev/")) { // Verificar si la linea tiene informacion sobre dispositivo de almacenamiento
            sscanf(path, "%*s %*s %*s %*s %lf%% %*s %*s", &porcentaje); // Extraer porcentaje de uso del disco de la linea
            sprintf(buffer, "Porcentaje de utilizacion del disco en MiB: %.2f%%\n", porcentaje); // Crear mensaje con el porcentaje
            write(pipefd[1], buffer, strlen(buffer)); // Escribir en el pipe
            break; // Terminar despues de encontrar la primera coincidencia
        }
    }

    // Cerrar el proceso y el pipe
    pclose(fp);
}

void obtenerPorcentajeUsoDiscoEnGiB(int pipefd[2]) {
    FILE *fp; // Declarar puntero a FILE para manejar el proceso que ejecutara el comando
    char path[1035]; // Buffer para almacenar cada linea de salida del comando
    char buffer[256]; // Buffer para almacenar el mensaje a enviar por el pipe
    double porcentaje; // Almacenar porcentaje de uso del disco

    // Ejecutar el comando df -BG y capturar su salida en modo GiB
    fp = popen("df -BG /", "r"); // Abrir proceso que ejecuta el comando en modo de lectura
    if (fp == NULL) { // Validacion
        perror("Error al ejecutar df -BG");
        exit(EXIT_FAILURE);
    }

    // Leer la salida línea por línea
    while (fgets(path, sizeof(path)-1, fp) != NULL) { // Leer cada linea de la salida del comando
        // Procesar la línea que contiene el espacio utilizado y disponible
        if (strstr(path, "/dev/")) { // Verificar si la linea tiene informacion sobre dispositivo de almacenamiento
            sscanf(path, "%*s %*s %*s %*s %lf%% %*s %*s", &porcentaje); // Extraer porcentaje de uso del disco de la linea
            sprintf(buffer, "Porcentaje de utilizacion del disco en GiB: %.2f%%\n", porcentaje); // Crear mensaje con el porcentaje
            write(pipefd[1], buffer, strlen(buffer)); // Escribir en el pipe
            break; // Terminar despues de encontrar la primera coincidencia
        }
    }

    // Cerrar el proceso 
    pclose(fp);
}

void obtenerPorcentajeEspacioLibreDiscoEnGiB(int pipefd[2]) {
    FILE *fp; // Declarar puntero a FILE para manejar el proceso que ejecutara el comando
    char path[1035]; // Buffer para almacenar cada linea de salida del comando
    char buffer[256]; // Buffer para almacenar el mensaje a enviar por el pipe
    double porcentaje; // Almacenar porcentaje de uso del disco

    // Ejecutar el comando df -BG y capturar su salida en modo GiB
    fp = popen("df -BG", "r"); // Abrir proceso que ejecuta el comando en modo de lectura
    if (fp == NULL) { // Validacion
        perror("Error al ejecutar df -BG");
        exit(EXIT_FAILURE);
    }

    // Leer la salida línea por línea
    while (fgets(path, sizeof(path)-1, fp) != NULL) { // Leer cada linea de la salida del comando
        // Procesar la línea que contiene el espacio utilizado y disponible
        if (strstr(path, "/dev/")) { // Verificar si la linea tiene informacion sobre dispositivo de almacenamiento
            sscanf(path, "%*s %*s %*s %*s %lf%% %*s %*s", &porcentaje); // Extraer porcentaje de uso del disco de la linea
            sprintf(buffer, "Porcentaje libre del disco en GiB: %.2f%%\n", 100-porcentaje); // Crear mensaje con el porcentaje
            write(pipefd[1], buffer, strlen(buffer)); // Escribir en pipe
            break; // Terminar después de encontrar la primera coincidencia
        }
    }

    // Cerrar el proceso
    pclose(fp);
}

void obtenerPorcentajeEspacioLibreDiscoEnMiB(int pipefd[2]) {
    FILE *fp; // Declarar puntero a FILE para manejar el proceso que ejecutara el comando
    char path[1035]; // Buffer para almacenar cada linea de salida del comando
    char buffer[256]; // Buffer para almacenar el mensaje a enviar por el pipe
    double porcentaje; // Almacenar porcentaje de uso del disco

    // Ejecutar el comando df -BM y capturar su salida en modo MiB
    fp = popen("df -BM", "r"); // Abrir proceso que ejecuta el comando en modo de lectura
    if (fp == NULL) { // Validacion
        perror("Error al ejecutar df -BM");
        exit(EXIT_FAILURE);
    }

    // Leer la salida línea por línea
    while (fgets(path, sizeof(path)-1, fp) != NULL) { // Leer cada linea de la salida del comando
        // Procesar la línea que contiene el espacio utilizado y disponible
        if (strstr(path, "/dev/")) { // Verificar si la linea tiene informacion sobre dispositivo de almacenamiento
            sscanf(path, "%*s %*s %*s %*s %lf%% %*s %*s", &porcentaje); // Extraer porcentaje de uso del disco de la linea
            sprintf(buffer, "Porcentaje libre del disco en MiB: %.2f%%\n", 100-porcentaje);  // Crear mensaje con el porcentaje
            write(pipefd[1], buffer, strlen(buffer)); // Escribir en pipe
            break; // Terminar despues de encontrar la primera coincidencia
        }
    }

    // Cerrar el proceso 
    pclose(fp);
}