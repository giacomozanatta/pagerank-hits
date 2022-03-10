main: main.o dataset.o csr.o pagerank.o utils.o 
	gcc -std=gnu89 -Wall -pedantic -g -o main main.o dataset.o utils.o csr.o pagerank.o
	rm -f *.o

main.o: main.c
	gcc -g -std=gnu89 -Wall -pedantic -c main.c

dataset.o: dataset.c dataset.h
	gcc -g -std=gnu89 -Wall -pedantic -c dataset.c
pagerank.o: pagerank.c pagerank.h
	gcc -g -std=gnu89 -Wall -pedantic -c pagerank.c
utils.o: utils.c utils.h
	gcc -g -std=gnu89 -Wall -pedantic -c utils.c