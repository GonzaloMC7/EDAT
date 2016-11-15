FLAGS= -pedantic -Wall -Wextra -ansi

all: usuario

usuario: usuario.o  odbc.o
	gcc -o usuario  usuario.o odbc.o -lodbc

usuario.o: usuario.c odbc.h
	gcc -c $(FLAGS)   usuario.c -lodbc

odbc.o: odbc.c odbc.h
	gcc  -c $(FLAGS) odbc.c -lodbc

clean:
	rm *.o
