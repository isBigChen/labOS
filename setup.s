BITS 16
org 0x8000

; define main function
; main function is in the third sector, 0x8000+512=0x8200 
%define c_main 8200h

; define gdt 
%define seg_null times 8 db 0

; set_seg(%1, %2, %3), type:, base, lim
%macro set_seg 3
    dw (((%3) >> 12) & 0xffff), ((%2) & 0xffff)
    db (((%2) >> 16) & 0xff), (0x90 | (%1)), \
       (0xC0 | (((%3) >> 28) & 0xf)), (((%2) >> 24) & 0xff)
%endmacro

; macro set_seg's %1 => type
%define STA_X 0x8     ; Executable segment
%define STA_E 0x4     ; Expand down (non-executable segments)
%define STA_C 0x4     ; Conforming code segment (executable only)
%define STA_W 0x2     ; Writeable (non-executable segments)
%define STA_R 0x2     ; Readable (executable segments)
%define STA_A 0x1     ; Accessed

; close intrupt
cli

wait1:
    in al, 64h  ; 64 port is 8042's status register
    test al, 02h
    jnz wait1       ; input buffer has data 
    mov al, 0xd1    ; 0xd1 => port 0x64
    out 64h, al     ; 
wait2:
    in al, 64h
    test al, 02h
    jnz wait2
    mov al, 0xdf    ; 0xdf => port 0x60
    out 60h, al     ; 0xdf = 11011111, open A20 gate

; load GDT
lgdt [temp_gdtdesc]

; cr0[0](PE) => 1, enter protect mode
mov eax, cr0
or eax, 1
mov cr0, eax

jmp dword 0x8:protect   ; jmpi 0x08 => gdt+1, eip=protect, 0x8 is PROT_MODE_CSEG

align 4    ; 4B align
temp_gdt:
    seg_null
    set_seg STA_X|STA_R, 0x0, 0xffffffff ; code segment
    set_seg STA_W, 0x0, 0xffffffff       ; data segment
temp_gdtdesc:
    dw 0x17         ; three segments, 3x8-1 = 23 = 0x17 = sizeof(gdt)-1
    dd temp_gdt     ; gdt start address


BITS 32

protect:
    ; init data segment registers
    mov ax, 10h     ; 0x10 is PROT_MODE_DSEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x0
    mov esp, 0x7c00 

jmp c_main
