all: fmt

fmt: fmt.c ascii.c Makefile
	gcc -O0 -fno-stack-protector -ggdb -o fmt fmt.c
	gcc -O0 -fno-stack-protector -ggdb -o ascii ascii.c
