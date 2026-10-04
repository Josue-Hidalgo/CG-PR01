BITS 64
default rel
section .text
global line_bresenham
extern plot

line_bresenham:
    push rbp
    mov rbp, rsp
    sub rsp, 64                  ; Pila alineada a 16 bytes antes de call.

    mov [rbp - 4], edi           ; x0
    mov [rbp - 8], esi           ; y0

    ; sx = (x1 > x0) - (x1 < x0)
    xor eax, eax
    xor r8d, r8d
    cmp edx, edi
    setg al
    setl r8b
    sub eax, r8d
    mov [rbp - 20], eax

    ; sy = (y1 > y0) - (y1 < y0)
    xor eax, eax
    xor r8d, r8d
    cmp ecx, esi
    setg al
    setl r8b
    sub eax, r8d
    mov [rbp - 24], eax

    ; dx = abs(x1 - x0)
    mov eax, edx
    sub eax, edi
    cdq
    xor eax, edx
    sub eax, edx
    mov [rbp - 12], eax

    ; dy = abs(y1 - y0)
    mov eax, ecx
    sub eax, esi
    cdq
    xor eax, edx
    sub eax, edx
    mov [rbp - 16], eax

    ; if (dx >= dy)
    mov eax, [rbp - 12]
    cmp eax, [rbp - 16]
    jl .y_dominante

    ; mayor=dx; menor=dy; paso=(sx,0); extra=(0,sy)
    mov [rbp - 28], eax
    mov eax, [rbp - 16]
    mov [rbp - 32], eax
    mov eax, [rbp - 20]
    mov [rbp - 36], eax
    mov dword [rbp - 40], 0
    mov dword [rbp - 44], 0
    mov eax, [rbp - 24]
    mov [rbp - 48], eax
    jmp .inicializar

.y_dominante:
    ; mayor=dy; menor=dx; paso=(0,sy); extra=(sx,0)
    mov [rbp - 32], eax
    mov eax, [rbp - 16]
    mov [rbp - 28], eax
    mov dword [rbp - 36], 0
    mov eax, [rbp - 24]
    mov [rbp - 40], eax
    mov eax, [rbp - 20]
    mov [rbp - 44], eax
    mov dword [rbp - 48], 0

.inicializar:
    ; delta_recto = 2 * menor
    mov eax, [rbp - 32]
    add eax, eax
    mov [rbp - 56], eax

    ; d = 2 * menor - mayor
    sub eax, [rbp - 28]
    mov [rbp - 52], eax

    ; delta_diagonal = 2 * (menor - mayor)
    mov eax, [rbp - 32]
    sub eax, [rbp - 28]
    add eax, eax
    mov [rbp - 60], eax

    ; Equivalente a for (i=0; i<mayor; i++): contar hacia cero.
    mov eax, [rbp - 28]
    mov [rbp - 64], eax

    ; plot(x0, y0). Estado en la pila: sobrevive a la llamada a C.
    mov edi, [rbp - 4]
    mov esi, [rbp - 8]
    call plot wrt ..plt

.ciclo:
    cmp dword [rbp - 64], 0
    jle .fin

    ; if (d < 0)
    cmp dword [rbp - 52], 0
    jl .recto

    ; else: x0+=extra_x; y0+=extra_y; d+=delta_diagonal
    mov eax, [rbp - 44]
    add [rbp - 4], eax
    mov eax, [rbp - 48]
    add [rbp - 8], eax
    mov eax, [rbp - 60]
    add [rbp - 52], eax
    jmp .avanzar

.recto:
    mov eax, [rbp - 56]
    add [rbp - 52], eax

.avanzar:
    ; x0 += paso_x; y0 += paso_y
    mov eax, [rbp - 36]
    add [rbp - 4], eax
    mov eax, [rbp - 40]
    add [rbp - 8], eax

    mov edi, [rbp - 4]
    mov esi, [rbp - 8]
    call plot wrt ..plt

    dec dword [rbp - 64]
    jmp .ciclo

.fin:
    leave
    ret

section .note.GNU-stack noalloc noexec nowrite progbits