FLAGS= -pedantic -Wall -Wextra -ansi

all: usuario oferta compra best_seller clean

usuario: usuario.o  odbc.o
	gcc -o usuario  usuario.o odbc.o -lodbc

oferta: oferta.o odbc.o
	gcc -o  oferta oferta.o odbc.o -lodbc

compra: compra.o odbc.o
	gcc -o compra compra.o odbc.o -lodbc

best_seller: best_seller.o odbc.o
	gcc -o best_seller best_seller.o odbc.o -lodbc

best_seller.o: best_seller.c odbc.h
	gcc -c $(FLAGS) best_seller.c -lodbc
	
compra.o: compra.c odbc.h
	gcc -c $(FLAGS) compra.c -lodbc

usuario.o: usuario.c odbc.h
	gcc -c $(FLAGS)   usuario.c -lodbc

oferta.o: oferta.c odbc.h
	gcc -c $(FLAGS) oferta.c -lodbc

odbc.o: odbc.c odbc.h
	gcc  -c $(FLAGS) odbc.c -lodbc

clean:
	rm *.o
