Section	.text

	global _start

_start:
	MOV rbx,0
	MOV rax,1

	int 0x80
