sivte 3, KEY_HANDLER

TERMINATE:
	jmp TERMINATE

KEY_HANDLER:
	rdpr 1, dr
	sub dr, 4
	add dr, SCAN_TO_CHAR

	mov ar, [dr]
	mov br, 0xFFFFFF
	int 0x1

	ret

SCAN_TO_CHAR:
	db "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"