BITS 16
; this is designed to be the first sector, it's code must be lower than 512B
; cs:ip=0000:7c00
org 0x7c00

; init segment registers
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov sp, 0x7c00

; print some message, use bios's intrupt vector fucntion
printStr:
    mov ax, 0x1301
    mov bx, 0x0002
    mov cx, len
    mov dx, 0x0101
    mov bp, msg
    int 0x10

; read sector to 0x8000
; buffer address  =>  es:bx = es<<4 + bx
next:
    mov ax, 0800h
    mov es, ax
    mov bx, 0
    mov ch, 0   ; Cylinder
    mov dh, 0   ; Heads
    mov cl, 2   ; Sector start number
    mov ah, 2   ; read function 
    mov al, 40  ; read 40 sectors
    mov dl, 80h ; 
    int 13h

    ; jump to setup.s
    jmp word 0000:8000h


; data to print
msg: db "oh~ hello labOS!", 10, 0
len equ 18

; padding with 0 to 510B
times 510-($-$$) db 0
; padding with 0x55aa, it represent gurb sector
dw 0xaa55
