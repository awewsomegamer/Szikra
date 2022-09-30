; Hello World application in Szikra assembly
; Write this code at 0x0 in the file
!AT 0x100

; Set BX to the pointer of the message 
mov bx, MESSAGE

; Print character at BX while the character is not 0
LOOP:
	mov ax, [bx]
	cmp ax, 0
	je END

	int 1

	add bx, 1

	jmp LOOP

; Termination loop
END:
	mov bx, MESSAGE
LOOPE:
	mov ax, [bx]
	cmp ax, 0
	je ENDE

	int 1

	add bx, 1

	jmp LOOPE
ENDE:
	jmp ENDE


; "Hello World\n\r"
MESSAGE:
	db "Hello Szikra World", 0x0


