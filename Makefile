#Regla para compilar el programa
programa: 
	@gcc -o programa programa.c -Wall

#Regla para eliminar el programa
clean:
	@rm programa
	