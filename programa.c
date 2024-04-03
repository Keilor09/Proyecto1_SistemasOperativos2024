#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "cpu.c"
#include "memoria.c"
#include "disco.c"

// Funcion principal que controla la obtencion de estadisticas
void obtenerEstadisticas(char* argv[]) {
    int pipefd[2];
    
    if (pipe(pipefd) == -1) { // Crear pipe
        printf("Error en pipe");
        exit(0);
    }

    pid_t pid = fork(); //Crear proceso hijo

    if (pid < 0) {
        printf("Error en creacion de hijo");
        exit(0);
    } else if (pid == 0) { // Proceso hijo

        if (strcmp(argv[1], "cpu") == 0) { // Compara el argumento recibido, si la funcion strcmp devuelve 0 quiere decir que es igual a CPU por lo que llama al metodo desarrollado en cpu.c
            obtenerUtilizacionCPU(pipefd);  
            close(pipefd[1]);          
        } else if (strcmp(argv[1], "memoria") == 0) {

            close(pipefd[1]); // Cierra extremo de escritura
        } else if (strcmp(argv[1], "disco") == 0 && argv[2] == NULL) {
            obtenerPorcentajeUsoDiscoEnGiB(pipefd);
            obtenerPorcentajeEspacioLibreDiscoEnGiB(pipefd);
            close(pipefd[1]);
        } else if (strcmp(argv[1], "disco") == 0 && strcmp(argv[2], "tm") == 0) {
            obtenerPorcentajeUsoDiscoEnMiB(pipefd);
            obtenerPorcentajeEspacioLibreDiscoEnMiB(pipefd);
            close(pipefd[1]);
        } else if (strcmp(argv[1], "disco") == 0 && strcmp(argv[2], "tg") == 0) {
            obtenerPorcentajeUsoDiscoEnGiB(pipefd);
            obtenerPorcentajeEspacioLibreDiscoEnGiB(pipefd);
            close(pipefd[1]);
        } else {
            printf("Opcion invalida\n"); // Argumento recibido no es valido
            exit(0);
        }
    } else { //Proceso padre
        wait(NULL);
        close(pipefd[1]); // Cerramos el extremo de escritura del pipe
        char buffer[1024]; // Almacenar estadisticas recibidas

        read(pipefd[0], buffer, sizeof(buffer)); // Leer desde el pide e imprimir estadisticas recibidas
        printf("%s\n", buffer);

        close(pipefd[0]); // Cerramos el extremo de lectura del pipe
    }
}

int main(int argc, char *argv[]) { // Funcion main
    obtenerEstadisticas(argv);  // Llama al metodo desarrollado anteriormente para obtener las estadisticas solicitadas utilizando pipes entre el padre e hijo
    return 0;
}