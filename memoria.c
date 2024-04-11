#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

void obtenerUtilizacionMemoria(int pipefd[2]) {
    DIR *directory; // Manejar directorio
    struct dirent *entry; // Manejar entradas del directorio
    FILE *file; // Manejar archivos
    char path[256], linea[256], nombre[256]; // Buffer para rutas de archivo, lineas leidas y nombres de procesos
    int pid; // pid de proceso
    long int vtam, totalMem = 0; // Tamaño mem virtual y total de mem virtual

    directory = opendir("/proc"); // Abrir proc para info de los procesos
    if (directory == NULL) { // Validar
        perror("No se pudo abrir /proc");
        return;
    }

    while ((entry = readdir(directory)) != NULL) { // Iterar entradas de /proc
        if ((pid = atoi(entry->d_name)) != 0) { // Verifica si es un proceso
            snprintf(path, sizeof(path), "/proc/%d/status", pid); // Construye ruta al status del proceso

            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Si no puede abrir el archivo continua (para que no se quede pegado en un archivo que no puede abrir por permisos, etc)
            }

            while (fgets(linea, sizeof(linea), file)) {
                if (sscanf(linea, "VmSize: %ld kB", &vtam) == 1) { // Busca VmSize para obtener la mem virtual del proceso
                    totalMem += vtam; // Suma al total global
                    break;
                }
            }
            fclose(file);
        }
    }
    closedir(directory);

    // Ahora, calcular el porcentaje de memoria utilizada por cada proceso
    directory = opendir("/proc");
    if (directory == NULL) {
        perror("Reapertura de /proc fallida");
        return;
    }

    while ((entry = readdir(directory)) != NULL) { // Itera de nuevo sobre cada proceso para calcular y reportar el uso de memoria
        if ((pid = atoi(entry->d_name)) != 0) { // Verifica si es un proceso
            snprintf(path, sizeof(path), "/proc/%d/stat", pid); // Construye ruta al stat del proceso
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Si no puede abrir el archivo continua (para que no se quede pegado en un archivo que no puede abrir por permisos, etc)
            }

            fscanf(file, "%*d %s", nombre); // Lee nombre del proceso
            fclose(file);

            snprintf(path, sizeof(path), "/proc/%d/status", pid); // construye ruta al stat del pid
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Si no puede abrir el archivo continua (para que no se quede pegado en un archivo que no puede abrir por permisos, etc)
            }

            while (fgets(linea, sizeof(linea), file)) { // Busca VmSize otra vez para obtener la mem virtual del proceso actual
                if (sscanf(linea, "VmSize: %ld kB", &vtam) == 1) {
                    float porcentaje = totalMem > 0 ? (float)vtam / totalMem * 100 : 0; // Calcula porcentaje, primero se verifica si totalMem es mayor que cero para evitar division entre 0
                                                                                        // Si totalMem = 0, se pone como resultado 0 para no dar datos erroneos
                    // Preparar buffer para escribir en el pipe la info recolectada
                    char buffer[256];
                    int n = snprintf(buffer, sizeof(buffer), "[V]%-10d %-20s %-20.2f%%\n", pid, nombre, porcentaje);

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


void obtenerUtilizacionMemoriaReal(int pipefd[2]) {
    DIR *directory; // Manejar directorio
    struct dirent *entry; // Manejar entradas de directorio
    FILE *file; // Manejar archivos
    char path[256], linea[256], nombre[256]; // Buffer para rutas del archivo, lineas leidas y nombre de procesos
    int pid; // pid de proceso
    long int rss, totalMemReal = 0; // Para memoria rss del proceso y la memoria real utilizada

    directory = opendir("/proc"); // Abrir /proc para acceder a la info de los procesos
    if (directory == NULL) { // Validar
        perror("No se pudo abrir /proc");
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {  // Verifica si es un proceso
            snprintf(path, sizeof(path), "/proc/%d/status", pid); // Construye ruta al status del proceso

            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Si no puede abrir el archivo continua (para que no se quede pegado en un archivo que no puede abrir por permisos, etc)
            }

            while (fgets(linea, sizeof(linea), file)) {
                if (sscanf(linea, "VmRSS: %ld kB", &rss) == 1) { // Busca VmRSS para obtener la mem RSS del proceso
                    totalMemReal += rss; // Suma rss al total global
                    break;
                }
            }
            fclose(file);
        }
    }
    closedir(directory);

    // Repite el proceso para calcular el porcentaje de memoria real utilizada por cada proceso.
    directory = opendir("/proc");
    if (directory == NULL) {
        perror("Reapertura de /proc fallida");
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {
            snprintf(path, sizeof(path), "/proc/%d/stat", pid);
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Solo continúa si no se puede abrir el archivo
            }

            fscanf(file, "%*d %s", nombre); // Lee nombre del proceso
            fclose(file);

            snprintf(path, sizeof(path), "/proc/%d/status", pid);
            file = fopen(path, "r");
            if (file == NULL) {
                continue; // Solo continúa si no se puede abrir el archivo
            }

            while (fgets(linea, sizeof(linea), file)) {
                if (sscanf(linea, "VmRSS: %ld kB", &rss) == 1) {
                    float porcentaje = totalMemReal > 0 ? (float)rss / totalMemReal * 100 : 0; // Calcula porcentaje, verifica primero si totalMemReal es mayor a 0 para evitar division por 0
                                                                                               // Si totalMemReal es 0 entonces deja unn 0 para evitar datos erroneos
                    // Preparar buffer con la info recolectada para escribir en pipe
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