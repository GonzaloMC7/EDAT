all: addUsr

addUsr: addUsr.o  odbc.o
	gcc -o addUsr  addUsr.o odbc.o -lodbc

addUsr.o: addUsr.c odbc.h
	gcc -c -pedantic -Wall -Wextra -ansi  addUsr.c -lodbc

odbc.o: odbc.c odbc.h
	gcc  -c -pedantic -Wall -Wextra -ansi odbc.c -lodbc

clean:
	rm *.o
