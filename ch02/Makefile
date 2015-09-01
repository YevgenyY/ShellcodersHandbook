all: function buffer buffer2 overflow serial shell find_start

function: Makefile
	gcc -O0 -g -o function function.c
buffer: Makefile
	gcc -O0 -g -o buffer buffer.c
buffer2: Makefile
	gcc -O0 -g -o buffer2 buffer2.c
overflow: Makefile
	gcc -O0 -fno-stack-protector -ggdb -o overflow overflow.c
serial: Makefile serial.c
	gcc -O0 -fno-stack-protector -ggdb -o serial serial.c
shell: Makefile shell.c shellcode.c
	gcc -O0 -fno-stack-protector -ggdb -o shell shell.c
	gcc -O0 -fno-stack-protector -ggdb -o shellcode shellcode.c
find_start: Makefile find_start.c
	gcc -O0 -fno-stack-protector -ggdb -o find-start find_start.c


