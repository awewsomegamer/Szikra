sivte 3, KEY_HANDLER
mov br, 0xFF

TERMINATE:
	jmp TERMINATE

KEY_HANDLER:
	rdpr 1, dr
	sub dr, 4
	add dr, SCAN_TO_CHAR

	mov ar, [dr]
	shl br, 1
	cmp br, 0xFFFFFFF
	jl OVER
	mov br, 0xFF
	OVER:

	int 0x1

	ret

SCAN_TO_CHAR:
	db "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"