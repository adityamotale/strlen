; Compile using `nasm -f elf64 strlen.asm -o strlen.o`

bits 64
default rel

section .text
    global strlen

; ------------------------------------------------------------
; size_t strlen(const char *str)
;
; System V AMD64 ABI
;   rdi = pointer to string
;   rax = length
; ------------------------------------------------------------
strlen:
    mov rax, rdi
    vpxord zmm1, zmm1, zmm1       

.loop:
    vmovdqu8 zmm0, [rdi]          
    
    vpcmpb k1, zmm0, zmm1, 0      
    kmovq rcx, k1                 

    test rcx, rcx
    jnz .found

    add rdi, 64
    jmp .loop

.found:
    tzcnt rcx, rcx                
    
    sub rdi, rax
    lea rax, [rdi + rcx]          
    
    vzeroupper
    ret
