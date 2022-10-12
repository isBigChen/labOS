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

; video mode
mov ah, 0x00    ; 设置显卡模式,同时会清屏
mov al, 0x02    ; 16色文本80x25
int 0x10

mov ah, 05h ; 设置显示活动页
mov al, 0   ; 为0
int 10h

; print some message, use bios's intrupt vector fucntion
printStr:
    mov ax, 0x1301  ; ah=13, al=01移动光标
    mov bx, 0x0002  ; bh=0第0页, bl=02显示属性
    mov cx, len 
    mov dx, 0   ; dh:0行,dl:0列
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
msg: db "oh~ hello labOS!", 0
len equ 16

; padding with 0 to 510B
times 510-($-$$) db 0
; padding with 0x55aa, it represent gurb sector
db 0x55, 0xaa
