output: programa.o ram.o cpu.o disco.o
	gcc programa.o -o programa -Wall	
	gcc ram.o -o ram -Wall
	gcc cpu.o -o cpu -Wall
	gcc disco.o -o disco -Wall
	sudo mv disco ram cpu programa /usr/local/bin

programa.o: programa.c
	gcc -c programa.c

memoria.o: ram.c
	gcc -c ram.c

cpu.o: cpu.c
	gcc -c cpu.c

disco.o: disco.c
	gcc -c disco.c

clean: 
	rm *.o
	sudo rm /usr/local/bin/cpu /usr/local/bin/programa /usr/local/bin/ram /usr/local/bin/disco
