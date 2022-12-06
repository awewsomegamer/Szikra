; sivte 3, KEY_HANDLE

; TERMINATE:
; 	jmp TERMINATE

; KEY_HANDLE:
; 	mov cr, 0x0
; 	mov ar, 'A'
; 	mov br, 0xFFFFFF
; 	int 1

; 	ret


SIVTE 0, timer_handler

TERMINATE:
	jmp TERMINATE

timer_handler:
	add i1, 1

	cmp i1, 1000
	jge update_screen

	ret

update_screen:
	mov cr, 0
	mov ar, 88
	mov br, 0xFF00000
	int 1
	mov i1, 0
	ret
