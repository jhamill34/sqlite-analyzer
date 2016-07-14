db: main.o list.o
	gcc -Wall -g -lsqlite3 -o main main.o list.o

main.o: main.c
	gcc -Wall -g -c main.c

list: list.o mainList.o
	gcc -Wall -g -o list list.o mainList.o

list.o: list.c
	gcc -Wall -g -c list.c

mainList.o: mainList.c
	gcc -Wall -g -c mainList.c

clean:
	rm -rf *.o list main
