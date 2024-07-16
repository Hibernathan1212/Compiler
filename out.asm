global _start
_start:
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    pop rax
    pop rbx
    mul rbx
    push rax
    mov rax, 10
    push rax
    pop rax
    pop rbx
    minus rax, rbx
    push rax
    pop rax
    pop rbx
    fslash rbx
    push rax
    mov rax, 0
    push rax
    push QWORD [rsp + 0]
    pop rax
    test rax, rax
    jz label0
    mov rax, 0
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
jmp label0
label0;
    mov rax, 1
    push rax
    pop rax
mov [rsp0], rax
    mov rax, 2
    push rax
    pop rax
mov [rsp0], rax
    mov rax, 3
    push rax
    pop rax
mov [rsp0], rax
    push QWORD [rsp + 0]
    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall