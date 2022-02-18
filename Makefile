main: main.o dataset.o utils.o
	gcc -std=gnu89 -Wall -pedantic -o main main.o dataset.o utils.o
	rm -f *.o

main.o: main.c dataset.h 
	gcc -std=gnu89 -Wall -pedantic -c main.c

dataset.o: dataset.c dataset.h
	gcc -std=gnu89 -Wall -pedantic -c dataset.c

utils.o: utils.c utils.h
	gcc -std=gnu89 -Wall -pedantic -c utils.c