main: main.o dataset.o csr.o ranking.o ranking.o utils.o 
	gcc -std=gnu89 -Wall -pedantic -O3 -o main main.o dataset.o utils.o csr.o ranking.o
	rm -f *.o
mmap: main.o dataset.o csr_mmap.o ranking.o ranking.o utils.o 
	gcc -std=gnu89 -Wall -pedantic -O3 -o  main main.o dataset.o utils.o csr.o ranking.o
	rm -f *.o
main.o: main.c
	gcc -g -std=gnu89 -Wall -pedantic -O3 -c main.c

dataset.o: dataset.c dataset.h
	gcc -g -std=gnu89 -Wall -pedantic -O3 -c dataset.c
ranking.o: ranking.c ranking.h
	gcc -g -std=gnu89 -Wall -pedantic -O3 -c ranking.c
utils.o: utils.c utils.h
	gcc -g -std=gnu89 -Wall -pedantic -O3 -c utils.c

csr_mmap.o: csr.c csr.h
	gcc -g -std=gnu89 -Wall -pedantic -O3 -c csr.c -DMMAP