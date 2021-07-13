


compil:register.c main.c register.h dechiffrement.c
	gcc  -O3 -pthread -g3  main.c  register.c dechiffrement.c -o main

clean:
	rm -Rf main