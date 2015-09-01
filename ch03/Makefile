all: exit 

exit: Makefile exit.c wack.c 
	gcc -O0 -fno-stack-protector -ggdb -o exit exit.c
	gcc -O0 -fno-stack-protector -ggdb -o wack wack.c
