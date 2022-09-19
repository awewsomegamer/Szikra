!AT 0

mov bx, MESSAGE

LOOP:
	mov ax, [bx]
	cmp ax, 0
	je END

	int 1
	
	add bx, 1

	jmp LOOP
	
END:
	jmp END

MESSAGE:
	db "Hello World", 0xA, 0xD, 0x0