FLAGS= -pedantic -Wall -Wextra -ansi

all: usuario oferta

usuario: usuario.o  odbc.o
	gcc -o usuario  usuario.o odbc.o -lodbc

oferta: oferta.o odbc.o
	gcc -o  oferta oferta.o odbc.o -lodbc

usuario.o: usuario.c odbc.h
	gcc -c $(FLAGS)   usuario.c -lodbc

oferta.o: oferta.c odbc.h
	gcc -c $(FLAGS) oferta.c -lodbc

odbc.o: odbc.c odbc.h
	gcc  -c $(FLAGS) odbc.c -lodbc

clean:
	rm *.o
