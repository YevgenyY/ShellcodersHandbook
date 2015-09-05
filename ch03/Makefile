all: exit 

exit: Makefile exit.c wack.c exit_shellcode.asm
	gcc -O0 -fno-stack-protector -ggdb -o exit exit.c
	gcc -O0 -fno-stack-protector -ggdb -z execstack -o wack wack.c
	nasm -f elf64 exit_shellcode.asm 
	ld -o exit_shellcode exit_shellcode.o
	nasm -f elf64 exit_shellcode2.asm 
	ld -o exit_shellcode2 exit_shellcode2.o
