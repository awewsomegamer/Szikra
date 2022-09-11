mov ax, 'A'

LOOP:
	int 1
	jmp LOOP

;LOOP:
	cmp ax, 0x5A
	jg END

	int 1

	add ax, 1

	jmp LOOP

END:
	jmp END;
