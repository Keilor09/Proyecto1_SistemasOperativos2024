#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "cpu.c"
#include "memoria.c"
#include "disco.c"
#include <string.h>

int main(int argc, char *argv[]) {
    
    if (strcmp(argv[1], "cpu") == 0) {  //Se utiliza la funcion strcmp para comparar el argumento guardado en argv[1] con cpu, si da 0 significa que son iguales
            pruebaCpu();
        }
        else if (strcmp(argv[1], "memoria") == 0) { //Se utiliza la funcion strcmp para comparar el argumento guardado en argv[1] con memoria, si da 0 significa que son iguales
            pruebaMemoria();
        }
        else if (strcmp(argv[1], "disco") == 0) { //Se utiliza la funcion strcmp para comparar el argumento guardado en argv[1] con disco, si da 0 significa que son iguales
            pruebaDisco();
        }
        else { //Validacion para que no ingrese otra opcion que no sea cpu, disco o memoria
            printf("Opcion invalida\n");
            return 1;
        }

    return 0;
}
