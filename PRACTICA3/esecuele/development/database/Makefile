all: test
	
test: testTableRecord.o table.o record.o type.o
	gcc -o test testTableRecord.o table.o record.o type.o

testTableRecord.o: testTableRecord.c table.h record.h ../type/type.h
	gcc -c testTableRecord.c
	
table.o: table.c table.h
	gcc -c table.c
	
record.o: record.c record.h
	gcc -c record.c
	
type.o: ../type/type.c ../type/type.h
	gcc -c ../type/type.c
	
clean:
	rm *.o