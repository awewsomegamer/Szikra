!AT 10

mov ax, 0x40
add ax, 1

int 1

mov ax, LABEL

; COMMENT

LABEL:  mov ax, 0 jmp 0x41 ; COMMENT


!AT LABEL mov ax, 0xAB

