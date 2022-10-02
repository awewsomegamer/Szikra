; Hello World application in Szikra assembly
; Write this code at 0x0 in the file
!AT 0x0

; Set BX to the pointer of the message 
mov cr, 1

START:
	mov br, MESSAGE

; Print character at BX while the character is not 0
LOOP:
	mov ar, [br]
	cmp ar, 0
	je END

	int 1

	add br, 1

	jmp LOOP

; Termination loop
END:	
	mov ar, '0'
	add ar, cr
	int 1
	mov ar, 0xA
	int 1

	add cr, 1
	cmp cr, 9
	jle START
TERMINATE:
	jmp TERMINATE

; "Hello World\n\r"
MESSAGE:
	db "Hello Szikra World", 0xA, 0xD, 0x0


