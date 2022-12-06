sivte 3, KEY_HANDLE

TERMINATE:
	jmp TERMINATE

KEY_HANDLE:
	mov cr, 0x0
	mov ar, 'A'
	mov br, 0xFFFFFF
	int 1

	ret