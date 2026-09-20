.global _main
.align 2
_fibonacci:
	stp x29, x30, [sp, #-16]!
	mov x29, sp
	sub sp, sp, #4080
	mov x10, x0
	str x10, [sp]
	ldr x11, [sp]
	cmp x11, #2
	cset x10, lt
	mov x11, x10
	cbz x11, .L_label_0
	ldr x0, [sp]
	mov sp, x29
	ldp x29, x30, [sp], #16
	ret
	b .L_label_1
.L_label_0:
.L_label_1:
	mov x10, #0
	str x10, [sp, #8]
	mov x10, #1
	str x10, [sp, #16]
	mov x10, #2
	str x10, [sp, #24]
	mov x10, #0
	str x10, [sp, #32]
.L_label_2:
	ldr x11, [sp, #24]
	ldr x12, [sp]
	cmp x11, x12
	cset x10, le
	mov x11, x10
	cbz x11, .L_label_3
	ldr x10, [sp, #8]
	ldr x12, [sp, #16]
	add x10, x10, x12
	mov x11, x10
	str x11, [sp, #32]
	ldr x10, [sp, #16]
	str x10, [sp, #8]
	ldr x10, [sp, #32]
	str x10, [sp, #16]
	ldr x10, [sp, #24]
	add x10, x10, #1
	mov x11, x10
	str x11, [sp, #24]
	b .L_label_2
.L_label_3:
	ldr x0, [sp, #16]
	mov sp, x29
	ldp x29, x30, [sp], #16
	ret
	mov sp, x29
	ldp x29, x30, [sp], #16
	ret
_print_fibonacci:
	stp x29, x30, [sp, #-16]!
	mov x29, sp
	sub sp, sp, #4080
	mov x9, x0
	str x9, [sp, #16]
	ldr x0, [sp, #16]
	bl _fibonacci
	mov x11, x0
	str x11, [sp, #8]
	ldr x10, [sp, #16]
	str x10, [sp, #0]
	adrp x0, mem_0@PAGE
	add x0, x0, mem_0@PAGEOFF
	bl _printf
	mov sp, x29
	ldp x29, x30, [sp], #16
	ret
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp
	sub sp, sp, #4080
	mov x9, x0
	str x9, [sp]
	mov x10, x1
	str x10, [sp, #8]
	mov x9, #0
	str x9, [sp, #16]
.L_label_4:
	ldr x10, [sp, #16]
	cmp x10, #20
	cset x9, le
	mov x10, x9
	cbz x10, .L_label_5
	ldr x0, [sp, #16]
	bl _print_fibonacci
	ldr x9, [sp, #16]
	add x9, x9, #1
	mov x10, x9
	str x10, [sp, #16]
	b .L_label_4
.L_label_5:
	mov x0, #0
	bl _exit
	mov sp, x29
	ldp x29, x30, [sp], #16
	ret
.data
.align 3
mem_0: .asciz "Fibonacci(%d) = %d!\n\0"
