Primer proyecto sistemas operativos 2024

Integrantes:

Keilor Aguilar Carazo
Reyder Fernández Mejias


Para compilar el proyecto:
make programa

Para eliminar el archivo complilado
make clean

En la parte de CPU:

./programa cpu 
Muestra el porcentaje de uso del CPU

./programa cpu pid
Muestra el porcentaje de uso del CPU del PID en los ultimos 5 minutos

En la parte de Memoria:

./programa memoria
Muestra el porcentaje de memoria virtual para todos los procesos

./programa memoria -r
Muestra el porcentaje de memoria real para todos los procesos 

./programa memoria -v
Muestra el porcentaje de memoria virtual para todos los procesos


En la parte de Disco:

./programa disco 
Muestra el porcentaje de utilizacion del disco en GiB

./programa disco -tm 
Muestra el porcentaje de utilzacion del disco en MiB

./programa disco -tg
Muestra el porcentaje de utilizacion de disco en GiB
