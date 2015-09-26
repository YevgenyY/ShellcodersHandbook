Section	.text

	global _start

_start:
	xor rbx,rbx
	mov al,1

	int 0x80
