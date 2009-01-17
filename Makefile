CC=gcc -g -Wall -lpcre
all: updatedb search
search: db.o db.h atrs.h search.o
	$(CC) db.o search.o -o search
updatedb: db.o db.h atrs.h updatedb.o
	$(CC) db.o updatedb.o -o updatedb
clean: 
	rm  *.o
