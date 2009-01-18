CC=gcc -g3 -Wall -lpcre
all: updatedb search
search: db.o db.h atrs.h search.o compress.o
	$(CC) db.o search.o compress.o -o search
updatedb: db.o db.h atrs.h updatedb.o compress.o
	$(CC) db.o updatedb.o compress.o -o updatedb
clean: 
	rm  *.o
