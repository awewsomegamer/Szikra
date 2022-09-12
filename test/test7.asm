mov bx, STRING


LOOP:
	mov ax, [bx]
	cmp ax, 0
	je END
	
	int 1
	
	add bx, 1

	jmp LOOP


END:
	int 1
	jmp END


STRING:
	ds "Hello World"
	db 0x0

;
mov ax, 65
int 1

LOOP:
	jmp LOOP
;
