default rel
BITS 64
section .text
global start
string_len:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov dword [rsp + 8], 0
	mov r9, qword [rsp]
	mov r8, r9
	mov r15d, dword [rsp + 8]
	mov r9d, r15d
	add r8, r9
	mov r11, r8
	mov r11, [r11]
	mov r12b, r11b
	mov r13d, 0
	cmp r12b, r13b
	setne al
	cmp al, 0
	je ._LBF1
._LBF0:
	mov r15d, dword [rsp + 8]
	mov r8d, r15d
	mov r9d, 1
	add r8, r9
	mov dword [rsp + 8], r8d
	mov r9, qword [rsp]
	mov r8, r9
	mov r15d, dword [rsp + 8]
	mov r9d, r15d
	add r8, r9
	mov r11, r8
	mov r11, [r11]
	mov r12b, r11b
	mov r13d, 0
	cmp r12b, r13b
	setne al
	cmp al, 0
	jne ._LBF0

._LBF1:
	mov r15d, dword [rsp + 8]
	mov eax, r15d
	pop rdi
	add rsp, 16
	pop rbp
	ret
exit:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov [syscall_safe + 8], rdi
	mov r9d, dword [rsp]
	mov edi, r9d
	mov [syscall_nowrite + 8], rdi
	mov rdi, [syscall_safe + 8]
	mov rdi, [syscall_nowrite + 8]
	mov rax, 0x2000001
	syscall
	mov rdi, [syscall_safe + 8]
	mov eax, 0
	pop rdi
	add rsp, 16
	pop rbp
	ret
print:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov [syscall_safe + 8], rdi
	mov [syscall_safe + 16], rsi
	mov [syscall_safe + 24], rdx
	mov edi, 1
	mov [syscall_nowrite + 8], rdi
	mov rdi, [syscall_safe + 8]
	mov r9, qword [rsp]
	mov rsi, r9
	mov [syscall_nowrite + 16], rsi
	mov rsi, [syscall_safe + 16]
	mov r9, qword [rsp]
	mov rdi, r9
	call string_len
	mov edx, eax
	mov [syscall_nowrite + 24], rdx
	mov rdx, [syscall_safe + 24]
	mov rdi, [syscall_nowrite + 8]
	mov rsi, [syscall_nowrite + 16]
	mov rdx, [syscall_nowrite + 24]
	mov rax, 33554436
	syscall
	mov rdi, [syscall_safe + 8]
	mov rsi, [syscall_safe + 16]
	mov rdx, [syscall_safe + 24]
	mov eax, 0
	pop rdi
	add rsp, 16
	pop rbp
	ret
printerr:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov [syscall_safe + 8], rdi
	mov [syscall_safe + 16], rsi
	mov [syscall_safe + 24], rdx
	mov edi, 2
	mov [syscall_nowrite + 8], rdi
	mov rdi, [syscall_safe + 8]
	mov r9, qword [rsp]
	mov rsi, r9
	mov [syscall_nowrite + 16], rsi
	mov rsi, [syscall_safe + 16]
	mov r9, qword [rsp]
	mov rdi, r9
	call string_len
	mov edx, eax
	mov [syscall_nowrite + 24], rdx
	mov rdx, [syscall_safe + 24]
	mov rdi, [syscall_nowrite + 8]
	mov rsi, [syscall_nowrite + 16]
	mov rdx, [syscall_nowrite + 24]
	mov rax, 33554436
	syscall
	mov rdi, [syscall_safe + 8]
	mov rsi, [syscall_safe + 16]
	mov rdx, [syscall_safe + 24]
	mov eax, 0
	pop rdi
	add rsp, 16
	pop rbp
	ret
malloc:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov [syscall_safe + 8], rdi
	mov [syscall_safe + 16], rsi
	mov [syscall_safe + 24], rdx
	mov [syscall_safe + 32], r10
	mov [syscall_safe + 40], r8
	mov [syscall_safe + 48], r9
	mov edi, 0
	mov [syscall_nowrite + 8], rdi
	mov rdi, [syscall_safe + 8]
	mov r9, qword [rsp]
	mov rsi, r9
	mov [syscall_nowrite + 16], rsi
	mov rsi, [syscall_safe + 16]
	mov edx, 3
	mov [syscall_nowrite + 24], rdx
	mov rdx, [syscall_safe + 24]
	mov r10d, 4098
	mov [syscall_nowrite + 32], r10
	mov r10, [syscall_safe + 32]
	mov r8d, -1
	mov [syscall_nowrite + 40], r8
	mov r8, [syscall_safe + 40]
	mov r9d, 0
	mov [syscall_nowrite + 48], r9
	mov r9, [syscall_safe + 48]
	mov rdi, [syscall_nowrite + 8]
	mov rsi, [syscall_nowrite + 16]
	mov rdx, [syscall_nowrite + 24]
	mov r10, [syscall_nowrite + 32]
	mov r8, [syscall_nowrite + 40]
	mov r9, [syscall_nowrite + 48]
	mov rax, 0x20000c5
	syscall
	mov rdi, [syscall_safe + 8]
	mov rsi, [syscall_safe + 16]
	mov rdx, [syscall_safe + 24]
	mov r10, [syscall_safe + 32]
	mov r8, [syscall_safe + 40]
	mov r9, [syscall_safe + 48]
	pop rdi
	add rsp, 16
	pop rbp
	ret
int_to_string:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	push rdi
	mov r9d, dword [rsp]
	mov r12d, r9d
	mov r13d, 0
	cmp r12d, r13d
	sete al
	cmp al, 0
	je ._LBF1
._LBF0:
	lea r10, [_LBC0]
	mov rax, r10
	pop rdi
	add rsp, 16
	pop rbp
	ret
	jmp ._LBF2
._LBF1:

._LBF2:
	mov edi, 100
	call malloc
	mov qword [rsp + 4], rax
	mov r9d, dword [rsp]
	mov dword [rsp + 12], r9d
	mov dword [rsp + 16], 0
	mov r9d, dword [rsp]
	mov r12d, r9d
	mov r13d, 0
	cmp r12d, r13d
	setl al
	cmp al, 0
	je ._LBF4
._LBF3:
	mov r8d, 0
	mov r15d, dword [rsp + 12]
	mov r9d, r15d
	sub r8, r9
	mov dword [rsp + 12], r8d
	jmp ._LBF5
._LBF4:

._LBF5:
	mov r15d, dword [rsp + 12]
	mov r12d, r15d
	mov r13d, 0
	cmp r12d, r13d
	setne al
	cmp al, 0
	je ._LBF7
._LBF6:
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 16]
	mov r9d, r15d
	add r8, r9
	mov r14, r8
	mov r15d, dword [rsp + 12]
	mov eax, r15d
	mov ebx, 10
	cdq
	idiv ebx
	mov r8d, edx
	mov r9d, 48
	add r8, r9
	mov dword [r14], r8d
	mov r15d, dword [rsp + 12]
	mov eax, r15d
	mov ebx, 10
	cdq
	idiv ebx
	mov dword [rsp + 12], eax
	mov r15d, dword [rsp + 16]
	mov r8d, r15d
	mov r9d, 1
	add r8, r9
	mov dword [rsp + 16], r8d
	mov r15d, dword [rsp + 12]
	mov r12d, r15d
	mov r13d, 0
	cmp r12d, r13d
	setne al
	cmp al, 0
	jne ._LBF6

._LBF7:
	mov r9d, dword [rsp]
	mov r12d, r9d
	mov r13d, 0
	cmp r12d, r13d
	setl al
	cmp al, 0
	je ._LBF9
._LBF8:
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 16]
	mov r9d, r15d
	add r8, r9
	mov r14, r8
	mov byte [r14], '-'
	mov r15d, dword [rsp + 16]
	mov r8d, r15d
	mov r9d, 1
	add r8, r9
	mov dword [rsp + 16], r8d
	jmp ._LBF10
._LBF9:

._LBF10:
	mov r15d, dword [rsp + 16]
	mov r8d, r15d
	mov r9d, 1
	sub r8, r9
	mov dword [rsp + 22], r8d
	mov dword [rsp + 26], 0
	mov r15d, dword [rsp + 26]
	mov r12d, r15d
	mov r15d, dword [rsp + 22]
	mov r13d, r15d
	cmp r12d, r13d
	setl al
	cmp al, 0
	je ._LBF12
._LBF11:
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 26]
	mov r9d, r15d
	add r8, r9
	mov r11, r8
	mov r11, [r11]
	mov byte [rsp + 12], r11b
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 26]
	mov r9d, r15d
	add r8, r9
	mov r14, r8
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 22]
	mov r9d, r15d
	add r8, r9
	mov r11, r8
	mov r11, [r11]
	mov byte [r14], r11b
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 22]
	mov r9d, r15d
	add r8, r9
	mov r14, r8
	mov r15d, dword [rsp + 12]
	mov dword [r14], r15d
	mov r15d, dword [rsp + 26]
	mov r8d, r15d
	mov r9d, 1
	add r8, r9
	mov dword [rsp + 26], r8d
	mov r15d, dword [rsp + 22]
	mov r8d, r15d
	mov r9d, 1
	sub r8, r9
	mov dword [rsp + 22], r8d
	mov r15d, dword [rsp + 26]
	mov r12d, r15d
	mov r15d, dword [rsp + 22]
	mov r13d, r15d
	cmp r12d, r13d
	setl al
	cmp al, 0
	jne ._LBF11

._LBF12:
	mov r15, qword [rsp + 4]
	mov r8, r15
	mov r15d, dword [rsp + 16]
	mov r9d, r15d
	add r8, r9
	mov r14, r8
	mov dword [r14], 0
	mov r15, qword [rsp + 4]
	mov rax, r15
	pop rdi
	add rsp, 16
	pop rbp
	ret
start:
	mov rdi, [rsp]
	lea rsi, [rsp + 8]
	push rbp
	mov rbp, rsp
	sub rsp, 32
	push rsi
	push rdi
	mov edi, 0
	mov rax, 0x2000001
	syscall
section .data
	_LBC0: db '0', 0
section .bss
	syscall_safe resq 10
	syscall_nowrite resq 10
