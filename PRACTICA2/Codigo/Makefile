FLAGS= -pedantic -Wall -Wextra -ansi

all: usuario oferta compra best_seller clean

usuario: usuario.o
	gcc -o usuario  usuario.o  -lodbc

oferta: oferta.o
	gcc -o  oferta oferta.o  -lodbc

compra: compra.o
	gcc -o compra compra.o  -lodbc

best_seller: best_seller.o
	gcc -o best_seller best_seller.o -lodbc

best_seller.o: best_seller.c
	gcc -c $(FLAGS) best_seller.c

compra.o: compra.c
	gcc -c $(FLAGS) compra.c

usuario.o: usuario.c 
	gcc -c $(FLAGS)   usuario.c

oferta.o: oferta.c
	gcc -c $(FLAGS) oferta.c



clean:
	rm *.o
