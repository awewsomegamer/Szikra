mov ax, 'A'

LOOP:
	int 1
	jmp LOOP

ds "Hello World"
db 0x0

;LOOP:
	cmp ax, 0x5A
	jg END

	int 1

	add ax, 1

	jmp LOOP

END:
	jmp END ;
