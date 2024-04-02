all:
	echo "cpu.c, memoria.c, disco.c"

build:
	gcc -o cpu cpu.c -Wall
	gcc -o memoria memoria.c -Wall
	gcc -o disco disco.c -Wall
	
run:
	./cpu
	./memoria
	./disco

clean:
	rm cpu
	rm memoria
	rm disco
	
	