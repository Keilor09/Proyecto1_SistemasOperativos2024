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
    long int vtam, total_vtam = 0, total_mem = 0; // Tamaño mem virtual, tamaño total de la vmem, mem total

    
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
                    total_mem += vtam;
                    break;
                }
            }
            fclose(file);

            
            total_vtam += vtam; // Acumula el tamaño de mem virtual

            
            float percentage = (float)vtam / total_mem * 100; // Calcula porcentaje de mem virtual

            
            char buffer[256]; // Convierte PID, nombre y porcentaje de mem virtual a cadena
            int n = sprintf(buffer, "%-10d %-20s %-20.2f%%\n", pid, nombre, percentage);

            
            write(pipefd[1], buffer, n); // Escribe en pipe
        }
    }

    closedir(directory); // Cierra directorio
}

