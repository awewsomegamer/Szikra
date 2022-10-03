sivte 3, KEY_HANDLER

TERMINATE:
	jmp TERMINATE

KEY_HANDLER:
	rdpr 1, br
	sub br, 4
	add br, SCAN_TO_CHAR
	mov ar, [br]
	int 0x1

	ret

SCAN_TO_CHAR:
	db "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890     "