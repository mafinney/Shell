shell: shell.o 
	gcc -g -o shell shell.o

test_shell: test_shell.o shell.o
	gcc -g -o test_shell test_shell.o shell.o

shell.o: shell.c shell.h
	gcc -g -c shell.c

test_shell.o: test_shell.c shell.h
	gcc -g -c test_shell.c

clean: 
	rm *.o shell