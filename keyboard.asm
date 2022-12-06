sivte 3, KEY

TERMINATE:
	jmp TERMINATE

KEY:
	rdpr 1, dr
	add dr, ALPHABET
	sub dr, 4
	mov ar, [dr]

	mov br, 0xFFFFFF

	int 0x1

	ret

ALPHABET:
	db "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
