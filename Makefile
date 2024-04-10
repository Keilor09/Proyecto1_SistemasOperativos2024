#Regla para compilar el programa, cpu, disco y memoria
programa: 
	@gcc -o programa programa.c -w


#Regla para eliminar el programa, cpu, disco y memoria
clean:
	@rm programa

	