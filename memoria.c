#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

void obtenerUtilizacionMemoriaVirtual(int pipefd[2]) {
    DIR *directory; // Para utilizar en directorio
    struct dirent *entry; // entraddas del directorio
    FILE *file; // para archivo
    char path[256], linea[256], nombre[256]; // buffer para el path, lineas leidas y nombres del prcesos
    int pid; // pid del proceso
    long int vsize, totalMemVirtual = 0, swapTotal = 0; // variables para almacenar la informacion de la memoria virtual del proceso y del sistema

    // Leer el total de memoria física y swap del sistema desde /proc/meminfo para obtener la mem virtual total
    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("No se pudo abrir /proc/meminfo");
        return;
    }

    while (fgets(linea, sizeof(linea), file)) {
        if (sscanf(linea, "MemTotal: %ld kB", &totalMemVirtual) == 1 || sscanf(linea, "SwapTotal: %ld kB", &swapTotal) == 1) {
            totalMemVirtual += swapTotal;  // Sumar el total de memoria y swap para obtener la memoria virtual total
        }
    }
    fclose(file);

    if (totalMemVirtual == 0) { // Validar que el valor de la memoria sea valido para evitar errores
        perror("No se pudo leer el total de memoria del sistema");
        return;
    }

    // Abrir /proc para leer la informacion de los procesos
    directory = opendir("/proc"); 
    if (directory == NULL) {
        perror("No se pudo abrir /proc");
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {
            snprintf(path, sizeof(path), "/proc/%d/status", pid); // construir ruta al archivvo del  status ddel proceso
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Continuar si el archivo no se puede abrir para evitar errores
            }

            while (fgets(linea, sizeof(linea), file)) {
                if (sscanf(linea, "VmSize: %ld kB", &vsize) == 1) { // Leer la VmSize que contiene la mem virtual del proceso
                    float porcentaje = (float)vsize / totalMemVirtual * 100; // Hacer el calculo para obtener el porcentajje de uso del proceso

                    // Leer nombre del proceso desde /proc/[pid]/stat
                    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
                    FILE *statFile = fopen(path, "r");
                    if (statFile == NULL) { // Si no se pudo abrir se continua para evitar errores
                        continue;
                    }
                    if (fscanf(statFile, "%*d (%[^)])", nombre) != 1) { // Mejorar extraccion del nombre para manejar espacios y parentesis
                        strcpy(nombre, "Desconocido");
                    }
                    fclose(statFile);

                    // Escribir en el pipe la información recolectada
                    char buffer[256];
                    int n = snprintf(buffer, sizeof(buffer), "[V]%-10d %-20s %-20.2f%%\n", pid, nombre, porcentaje);
                    write(pipefd[1], buffer, n);
                    break;
                }
            }
            fclose(file);
        }
    }
    closedir(directory);
    close(pipefd[1]);
}

void obtenerUtilizacionMemoriaReal(int pipefd[2]) {
    DIR *directory; // Para el directorio
    struct dirent *entry; // manejar entradas del ddirectorio
    FILE *file; // manejar archivos
    char path[256], linea[256], nombre[256]; // buffer para el path, linea leida y nombre del pid
    int pid; // pid del proceso
    long int rss, totalMemReal = 0; // variables para almacenar la rss y la mem total del sistema

    // Primero, leer el total de memoria real del sistema desde /proc/meminfo
    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("No se pudo abrir /proc/meminfo");
        return;
    }
    while (fgets(linea, sizeof(linea), file)) {
        if (sscanf(linea, "MemTotal: %ld kB", &totalMemReal) == 1) { // Buscar la linea que tiene el MemTotal para encontrar la mem total real del sistema
            break;
        }
    }
    fclose(file);

    // Abrir /proc para leer la informacion de los procesos
    directory = opendir("/proc");
    if (directory == NULL) { // validar para evitar errores
        perror("No se pudo abrir /proc");
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {  // Verifica si es un proceso
            snprintf(path, sizeof(path), "/proc/%d/stat", pid); // Contruir ruta al archivo que tiene el stat ddel proceso
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Continua si no se puede abrir el archivo
            }
            if (fscanf(file, "%*d (%[^)])", nombre) != 1) { // Extrae el nombre del proceso correctamente teniendo en cuenta espacios y parentesis
                strcpy(nombre, "Desconocido"); // Si falla la lectura del nombre, asigna un valor predeterminado
            }
            fclose(file);

            snprintf(path, sizeof(path), "/proc/%d/status", pid); // Construir ruta la archivo que tiene status del proceso
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Continua si no se puede abrir el archivo
            }
            while (fgets(linea, sizeof(linea), file)) {
                if (sscanf(linea, "VmRSS: %ld kB", &rss) == 1) { // Obtiene el rss para la memoria ddel proceso
                    float porcentaje = totalMemReal > 0 ? (float)rss / totalMemReal * 100 : 0; // Calcula el porcentaje de meem real del prceso
                    // Preparar buffer con la info recolectada para escribir en el pipe
                    char buffer[256];
                    int n = snprintf(buffer, sizeof(buffer), "[R]%-10d %-20s %-20.2f%%\n", pid, nombre, porcentaje);
                    write(pipefd[1], buffer, n); // Escribe en el pipe
                    break;
                }
            }
            fclose(file);
        }
    }
    closedir(directory);
    close(pipefd[1]); // Cierra el pipe
}