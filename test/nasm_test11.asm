mov ebx, MESSAGE

LOOP:
	mov eax, [ebx]
	cmp eax, 0
	je END
	
	int 1

	add ebx, 1

	jmp LOOP

END:
	jmp END

MESSAGE:
	db "Hello Szikra World", 0xA, 0xD, 0x0