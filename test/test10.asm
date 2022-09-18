!AT 0

mov bx, MESSAGE

; LOOP:
	mov ax, [bx]


END:
	jmp END

MESSAGE:
	db "Hello World"
	db 0x0