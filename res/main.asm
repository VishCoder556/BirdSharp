default rel
BITS 64
section .bss align=16
	a: resd 1
section .data align=8
	_LBC0: db '0', 0
	_LBC1: db '', 10, '', 0
section .text align=16
global start
__pre:
	ret
string_len:
	xor rax, rax
.loop:
	cmp byte [rdi], 0
	je .done
	inc rdi
	inc rax
	jnz .loop
.done:
	ret
malloc:
	mov rsi, rdi            ; length
	xor rdi, rdi
	mov rdx, 0x3             ; PROT_READ | PROT_WRITE
	mov r10, 0x1002          ; MAP_PRIVATE | MAP_ANON
	mov r8, -1               ; fd = -1 (not used)
	xor r9, r9               ; offset = 0
	mov rax, 0x20000C5       ; syscall number for mmap
	syscall
	ret
print:
	mov rax, 0
	mov rcx, rdi
	call string_len
	mov rdi, rcx
	mov rdx, rax
	mov rsi, rdi
	mov rdi, 1
	mov rax, 0x02000004
	syscall
	ret

make_str:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	push rdi
	mov rdi, 2
	call malloc
	pop rdi
	mov qword [rbp - 32], rax
	
	mov r12, 48
	mov r11b, dil
	mov r13b, r12b
	add r13b, r11b
	mov r8, qword [rbp - 32]
	mov byte [r8], r13b
	mov r13b, 0
	mov r12, 1
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	mov rax, qword [rbp - 32]
	mov rsp, rbp
	pop rbp
	ret
	mov rsp, rbp
	pop rbp
	ret
char_to_string:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	push rdi
	mov rdi, 2
	call malloc
	pop rdi
	mov qword [rbp - 32], rax
	
	mov r13b, dil
	mov r8, qword [rbp - 32]
	mov byte [r8], r13b
	mov r13b, 0
	mov r12, 1
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	mov rax, qword [rbp - 32]
	mov rsp, rbp
	pop rbp
	ret
	mov rsp, rbp
	pop rbp
	ret
int_to_string:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov r9, 0
	mov r10d, edi
	cmp r10, r9

	jne ._LBF2
	._LBF0:
	lea r13, [_LBC0]
	mov rax, r13
	mov rsp, rbp
	pop rbp
	ret
	jmp ._LBF1
	
._LBF2:
._LBF3:jmp ._LBF1

._LBF1:
	push rdi
	mov rdi, 100
	call malloc
	pop rdi
	mov qword [rbp - 32], rax
	
	mov dword [rbp - 24], edi
	mov dword [rbp - 20], 0
	mov r9, 0
	mov r10d, dword [rbp - 24]
	cmp r10, r9
	setne al
	movzx r13, al
	
	test r13, r13
	jz ._LBF5
._LBF4:
	mov r12, 48
	push rax
	push rdx
	mov eax, dword [rbp - 24]
	mov r13, 10
	cqo
	idiv r13
	mov r11d, edx
	pop rdx
	pop rax
	mov r13d, r12d
	add r13d, r11d
	mov r12d, dword [rbp - 20]
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	push rax
	push rdx
	mov eax, dword [rbp - 24]
	mov r13, 10
	cqo
	idiv r13
	mov dword [rbp - 24], eax
	pop rdx
	pop rax
	mov r12, 1
	mov r11d, dword [rbp - 20]
	mov dword [rbp - 20], r12d
	add dword [rbp - 20], r11d
	mov r9, 0
	mov r10d, dword [rbp - 24]
	cmp r10, r9
	setne al
	movzx r13, al
	
	cmp r13, 0
	jne ._LBF4
	jmp ._LBF5
	
._LBF5:
	mov r13b, 0
	mov r12d, dword [rbp - 20]
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	mov r14, 1
	mov r13d, dword [rbp - 20]
	mov dword [rbp - 20], r13d
	sub dword [rbp - 20], r14d
	mov dword [rbp - 16], 0
	mov r9d, dword [rbp - 20]
	mov r10d, dword [rbp - 16]
	cmp r10, r9
	setl al
	movzx r13, al
	
	test r13, r13
	jz ._LBF7
._LBF6:
	mov r12d, dword [rbp - 16]
	mov r11, qword [rbp - 32]
	mov r9, r12
	add r9, r11
	mov r10b, byte [r9]
	mov byte [rbp - 24], r10b
	mov r12d, dword [rbp - 20]
	mov r11, qword [rbp - 32]
	mov r9, r12
	add r9, r11
	mov r10b, byte [r9]
	mov r13b, r10b
	mov r12d, dword [rbp - 16]
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	mov r13d, dword [rbp - 24]
	mov r12d, dword [rbp - 20]
	mov r11, qword [rbp - 32]
	mov r8, r12
	add r8, r11
	mov byte [r8], r13b
	mov r12, 1
	mov r11d, dword [rbp - 16]
	mov dword [rbp - 16], r12d
	add dword [rbp - 16], r11d
	mov r14, 1
	mov r13d, dword [rbp - 20]
	mov dword [rbp - 20], r13d
	sub dword [rbp - 20], r14d
	mov r9d, dword [rbp - 20]
	mov r10d, dword [rbp - 16]
	cmp r10, r9
	setl al
	movzx r13, al
	
	cmp r13, 0
	jne ._LBF6
	jmp ._LBF7
	
._LBF7:
	mov rax, qword [rbp - 32]
	mov rsp, rbp
	pop rbp
	ret
	mov rsp, rbp
	pop rbp
	ret
align 16
start:
	mov rbx, rsp
	push rbp
	mov rbp, rsp
	sub rsp, 32
	call __pre
	mov dword [a], 9
	push rdi
	push rdi
	mov edi, dword [a]
	call int_to_string
	pop rdi
	mov rdi, rax
	
	call print
	pop rdi
	mov dword [rbp - 32], 0
	mov r9d, [rbx]
	mov r10d, dword [rbp - 32]
	cmp r10, r9
	setl al
	movzx r13, al
	
	test r13, r13
	jz ._LBF1
._LBF0:
	push rdi
	mov r14, 8
	mov r13d, dword [rbp - 32]
	imul r13, r14
	mov r12d, r13d
	mov r11, rbx
	add r11, 8
	mov r9, r12
	add r9, r11
	mov r10, qword [r9]
	mov rdi, r10
	call print
	pop rdi
	push rdi
	lea r13, [_LBC1]
	mov rdi, r13
	call print
	pop rdi
	mov r12, 1
	mov r11d, dword [rbp - 32]
	mov dword [rbp - 32], r12d
	add dword [rbp - 32], r11d
	mov r9d, [rbx]
	mov r10d, dword [rbp - 32]
	cmp r10, r9
	setl al
	movzx r13, al
	
	cmp r13, 0
	jne ._LBF0
	jmp ._LBF1
	
._LBF1:
	mov rsp, rbp
	pop rbp
	mov rax, 0x02000001
	xor rdi, rdi
	syscall
