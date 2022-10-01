; Hello World application in Szikra assembly
; Write this code at 0x0 in the file
!AT 0x0

; Set BX to the pointer of the message 
mov cx, 1

START:
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
	mov ax, '0'
	add ax, cx
	int 1
	mov ax, 0xA
	int 1

	add cx, 1
	cmp cx, 9
	jle START
TERMINATE:
	jmp TERMINATE

; "Hello World\n\r"
MESSAGE:
	db "Hello Szikra World", 0xA, 0xD, 0x0


