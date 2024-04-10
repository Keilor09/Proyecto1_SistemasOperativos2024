#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

void obtenerUtilizacionMemoria(int pipefd[2]) {
    DIR *directory; // Estructura directorio
    struct dirent *entry; // Entrada de directorio
    FILE *file; // Archivo
    char path[40], linea[100], nombre[100]; // Ruta de archivo, linea y nombre del proceso
    int pid; // id proceso
    long int vtam, totalVtam = 0, totalMem = 0; // Tamaño mem virtual, tamaño total de la vmem, mem total

    
    directory = opendir("/proc"); // Abrir directorio proc
    if (directory == NULL) { // Validar
        perror("No se pudo abrir /proc");
        return;
    }

    // Lee cada entrada del directorio /proc
    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {  // Comprueba si la entrada es un id del proceso
            sprintf(path, "/proc/%d/stat", pid); // Construye ruta al archivo /proc/pid/stat

            file = fopen(path, "r"); //Abre /proc/pid/stat
            if (file == NULL) { // validar
                perror("No se pudo abrir /proc/[pid]/stat");
                return;
            }

            fscanf(file, "%*d %s", nombre); // Lee nombre del proceso en stat
            fclose(file);

            
            sprintf(path, "/proc/%d/status", pid); // construye ruta a /proc/pid/status 

            file = fopen(path, "r"); // Abre archivo /proc/id/status
            if (file == NULL) { // Valida
                perror("Unable to open /proc/[pid]/status");
                return;
            }

            
            while (fgets(linea, sizeof(linea), file)) { // Lee campo de VmSize en status
                if (sscanf(linea, "VmSize: %ld kB", &vtam) == 1) {
                    totalMem += vtam;
                    break;
                }
            }
            fclose(file);

            
            totalVtam += vtam; // Acumula el tamaño de mem virtual

            
            float porcentaje = (float)vtam / totalMem * 100; // Calcula porcentaje de mem virtual

            
            char buffer[256]; // Convierte PID, nombre y porcentaje de mem virtual a cadena
            int n = sprintf(buffer, "[V]%-10d %-20s %-20.2f%%\n", pid, nombre, porcentaje);

            
            write(pipefd[1], buffer, n); // Escribe en pipe
        }
    }

    closedir(directory); // Cierra directorio
    close(pipefd[1]);
}


void obtenerUtilizacionMemoriaReal(int pipefd[2]) {
    DIR *directory;
    struct dirent *entry;
    FILE *file;
    char path[40], linea[100], nombre[100];
    int pid;
    long int size, resident, share, text, lib, data, dt;

    // Abrir el directorio /proc
    directory = opendir("/proc");
    if (directory == NULL) {
        perror("No se pudo abrir /proc");
        return;
    }

    // Leer cada entrada en el directorio /proc
    while ((entry = readdir(directory)) != NULL) {
        if ((pid = atoi(entry->d_name)) != 0) {  // Comprobar si la entrada es un ID de proceso
            // Construir la ruta al archivo /proc/[pid]/statm
            sprintf(path, "/proc/%d/statm", pid);

            // Abrir el archivo /proc/[pid]/statm
            file = fopen(path, "r");
            if (file == NULL) {
                perror("No se pudo abrir /proc/[pid]/statm");
                return;
            }

            // Leer el tamaño total de la memoria del archivo statm
            if (fscanf(file, "%ld %ld %ld %ld %ld %ld %ld", &size, &resident, &share, &text, &lib, &data, &dt) != 7) {
                perror("Error al leer /proc/[pid]/statm");
                fclose(file);
                continue; // Pasar al siguiente proceso si no se puede leer correctamente
            }
            fclose(file);

            // Calcular el tamaño total de la memoria utilizada (residente + swap)
            long int totalMem = resident + dt; // resident es la memoria residente y dt es la memoria swap

            // Construir la ruta al archivo /proc/[pid]/status
            sprintf(path, "/proc/%d/status", pid);

            // Abrir el archivo /proc/[pid]/status
            file = fopen(path, "r");
            if (file == NULL) {
                perror("No se pudo abrir /proc/[pid]/status");
                return;
            }

            // Leer el nombre del proceso del archivo status
            if (fgets(linea, sizeof(linea), file) == NULL) {
                perror("Error al leer /proc/[pid]/status");
                fclose(file);
                continue; // Pasar al siguiente proceso si no se puede leer correctamente
            }
            // Buscar el nombre del proceso
            sscanf(linea, "Name: %s", nombre);
            fclose(file);

            // Convertir el ID del proceso, nombre y porcentaje de memoria permanente a cadena
            char buffer[256];
            float porcentaje = (float)totalMem / size * 100;
            int n = sprintf(buffer, "[R]%-10d %-20s %-20.2f%%\n", pid, nombre, porcentaje);

            // Escribir la cadena en el pipe
            write(pipefd[1], buffer, n);
        }
    }

    closedir(directory);
    close(pipefd[1]); // Cerrar el extremo de escritura del pipe al finalizar
}