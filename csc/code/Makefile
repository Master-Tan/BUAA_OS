fibo: fibo.c main.c
	gcc -c fibo.c -I ../include
	gcc -c main.c -I ../include
	gcc main.c fibo.c -o ../fibo -I ../include

clean:
	rm fibo.o
	rm main.o
