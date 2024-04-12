#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include "cpu.c"
#include "disco.c"
#include "memoria.c"

// Prototipos de las funciones externas (definidas en sus respectivos archivos (cpu.c, memoria.c, disco.c))
void obtenerUtilizacionCPU(int pipefd[]);
void obtenerUtilizacionCPUProceso(int pipefd[], char* proceso);
void obtenerUtilizacionMemoriaVirtual(int pipefd[]);
void obtenerUtilizacionMemoriaReal(int pipefd[]);
void obtenerPorcentajeUsoDiscoEnGiB(int pipefd[]);
void obtenerPorcentajeEspacioLibreDiscoEnGiB(int pipefd[]);
void obtenerPorcentajeUsoDiscoEnMiB(int pipefd[]);
void obtenerPorcentajeEspacioLibreDiscoEnMiB(int pipefd[]);

void manejarCPU(int pipefd[2], char *argv[]) { // Manejar opciones del CPU
    if (argv[2] == NULL) { // Si no se proporciona ningun argumento se llama a la funcion que obtiene la utilizacion del CPU en general 
        obtenerUtilizacionCPU(pipefd);
    } else { // Si se proporciona un argumento se llama al metodo que obtiene la utilizacion del CPU en los ultimos 5 minutos de un PID especifico (en este caso seria el argumento)
        obtenerUtilizacionCPUProceso(pipefd, argv[2]);
    }
}

void manejarMemoria(int pipefd[2], char *argv[]) { // Manejar opciones del CPU
    if (argv[2] == NULL) { // Si no se proporciona ningun argumento se llama a la funcion que obtiene la utilizacion de memoria virtual para todos los procesos
        obtenerUtilizacionMemoriaVirtual(pipefd);
    } else if (strcmp(argv[2], "-r") == 0) { // Si se proporciona -r como argumento se llama a la funcion que obtiene la utilizacion de memoria real de todos los procesos
        obtenerUtilizacionMemoriaReal(pipefd);
    } else if (strcmp(argv[2], "-v") == 0) { // Si se proporciona -v como argumento se llama a la funcion que obtiene la utilizacion de memoria virtual de todos los procesos
        obtenerUtilizacionMemoriaVirtual(pipefd);
    } else { // Validar opcion
        printf("Argumento no reconocido para memoria\n");
        exit(EXIT_FAILURE);
    }
}

void manejarDisco(int pipefd[2], char *argv[]) { // Manejar opciones de Disco
    if (argv[2] == NULL) { // Si no se dan argumentos se llama por defecto al metodo que obtiene la informacion de porcentaje de uso/libre del disco en GiB
        obtenerPorcentajeUsoDiscoEnGiB(pipefd);
        obtenerPorcentajeEspacioLibreDiscoEnGiB(pipefd);
    } else if (strcmp(argv[2], "-tm") == 0) { // Si el argumento es -tm se llama al metodo que obtiene la informacion de porcentaje de uso/libre en MiB
        obtenerPorcentajeUsoDiscoEnMiB(pipefd);
        obtenerPorcentajeEspacioLibreDiscoEnMiB(pipefd);
    } else if (strcmp(argv[2], "-tg") == 0) { // Si el argumento es -tg se llama al metodo que obtiene la informacion de porcentaje de uso/libre en GiB
        obtenerPorcentajeUsoDiscoEnGiB(pipefd);
        obtenerPorcentajeEspacioLibreDiscoEnGiB(pipefd);
    } else { // Validar opcion
        printf("Argumento no reconocido para disco\n");
        exit(EXIT_FAILURE);
    }
}

void obtenerEstadisticas(char* argv[]) { // Metodo que obtiene las estadisticas solicitadas utilizando pipes entre padre e hijo
    int pipefd[2]; // Crear pipe para escribir y leer
    
    if (pipe(pipefd) == -1) { // Validar si hay error en la creacion del pipe
        printf("Error en pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // Creacion de hijo que se encargara de obtener datos de cpu, memoria y disco

    if (pid < 0) { // Validar creacion del hijo
        printf("Error en creacion de hijo\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Hijo obteniendo la informacion depediendo de los argumentos de la llamada al programa
        close(pipefd[0]); // Cerrar extremo de lectura del pipe
        if (strcmp(argv[1], "cpu") == 0) { // CPU
            manejarCPU(pipefd, argv); // Llama al metodo declarado anteriormente que maneja la info del cpu, envia como argumentos al pipe y el argumento escrito por el usuario
        } else if (strcmp(argv[1], "memoria") == 0) { // Memoria
            manejarMemoria(pipefd, argv); // Llama al metodo declarado anteriormente que maneja la info de la memoria, envia como argumentos al pipe y el argumento escrito por el usuario
        } else if (strcmp(argv[1], "disco") == 0) { // Disco
            manejarDisco(pipefd, argv); // Llama al metodo declarado anteriormente que maneja la info del disco, envia como argumentos al pipe y el argumento escrito por el usuario
        } else { // Validar opcion
            printf("Opcion invalida\n");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]); // Cerrar extremo de escritura del pipe
    } else {
        close(pipefd[1]); // Cerrar extremo de escritura del pipe
        wait(NULL); // Esperear a que termine el hijo
        char buffer[1024]; // Buffer para leer la info que envia el hijo
        read(pipefd[0], buffer, sizeof(buffer)); // Leer el pipe
        printf("%s\n", buffer); // Imprimir el buffer con la info que ha enviado el hijo 
        close(pipefd[0]); // Cerrar extremo de lectura del pipe
    }
}

int main(int argc, char *argv[]) { // Main
    if (argc > 1) {
        obtenerEstadisticas(argv); // Llamar al metodo de obtener estadisticas declarado anteriormente con el argumento escrito por el usuario
    } else { // Si no escribe argumento se le pide que escriba uno
        printf("Por favor proporcione un argumento\n");
    }
    return 0;
}
