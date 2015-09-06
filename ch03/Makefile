all: exit spawnshell

exit: Makefile exit.c wack.c exit_shellcode.asm
	gcc -O0 -fno-stack-protector -ggdb -o exit exit.c
	gcc -O0 -fno-stack-protector -ggdb -z execstack -o wack wack.c
	nasm -f elf64 exit_shellcode.asm 
	ld -o exit_shellcode exit_shellcode.o
	nasm -f elf64 exit_shellcode2.asm 
	ld -o exit_shellcode2 exit_shellcode2.o

spawnshell: Makefile execve2.c
	as --64 execve2.asm -o execve2.o
	ld -o execve2 execve2.o
	gcc -fno-stack-protector -static -ggdb -o spawnshell spawnshell.c
	gcc -O0 -fno-stack-protector -ggdb -z execstack -o execve execve2.c


