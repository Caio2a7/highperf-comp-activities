	.file	"task2.c"
	.text
	.globl	time_elapsed_ms
	.type	time_elapsed_ms, @function
time_elapsed_ms:
.LFB11:
	.cfi_startproc
	subq	%rdi, %rdx
	pxor	%xmm0, %xmm0
	cvtsi2sdq	%rdx, %xmm0
	mulsd	.LC0(%rip), %xmm0
	subq	%rsi, %rcx
	pxor	%xmm1, %xmm1
	cvtsi2sdq	%rcx, %xmm1
	divsd	.LC1(%rip), %xmm1
	addsd	%xmm1, %xmm0
	ret
	.cfi_endproc
.LFE11:
	.size	time_elapsed_ms, .-time_elapsed_ms
	.globl	vector_init
	.type	vector_init, @function
vector_init:
.LFB12:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L2
	leal	1(%rdi,%rdi), %edx
	movl	$1, %eax
	.p2align 4
.L4:
	movl	%eax, (%rsi)
	addl	$2, %eax
	addq	$4, %rsi
	cmpl	%edx, %eax
	jne	.L4
.L2:
	ret
	.cfi_endproc
.LFE12:
	.size	vector_init, .-vector_init
	.globl	cumulative_sum
	.type	cumulative_sum, @function
cumulative_sum:
.LFB13:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L9
	movq	%rsi, %rax
	movslq	%edi, %rdi
	leaq	(%rsi,%rdi,4), %rcx
	movl	$0, %edx
	.p2align 4
.L8:
	addl	(%rax), %edx
	addq	$4, %rax
	cmpq	%rcx, %rax
	jne	.L8
.L6:
	movl	%edx, %eax
	ret
.L9:
	movl	$0, %edx
	jmp	.L6
	.cfi_endproc
.LFE13:
	.size	cumulative_sum, .-cumulative_sum
	.globl	multiple_vars_sum
	.type	multiple_vars_sum, @function
multiple_vars_sum:
.LFB14:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	testl	%edi, %edi
	jle	.L14
	movq	%rsi, %rax
	subl	$1, %edi
	shrl	$3, %edi
	movl	%edi, %edi
	salq	$5, %rdi
	leaq	32(%rsi,%rdi), %rbx
	movl	$0, %ecx
	movl	$0, %esi
	movl	$0, %edi
	movl	$0, %r8d
	movl	$0, %r9d
	movl	$0, %r10d
	movl	$0, %r11d
	movl	$0, %edx
	.p2align 6
.L13:
	addl	(%rax), %edx
	addl	4(%rax), %r11d
	addl	8(%rax), %r10d
	addl	12(%rax), %r9d
	addl	16(%rax), %r8d
	addl	20(%rax), %edi
	addl	24(%rax), %esi
	addl	28(%rax), %ecx
	addq	$32, %rax
	cmpq	%rbx, %rax
	jne	.L13
.L12:
	leal	(%rdx,%r11), %eax
	addl	%r10d, %eax
	addl	%r9d, %eax
	addl	%r8d, %eax
	addl	%edi, %eax
	addl	%esi, %eax
	addl	%ecx, %eax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L14:
	.cfi_restore_state
	movl	$0, %ecx
	movl	$0, %esi
	movl	$0, %edi
	movl	$0, %r8d
	movl	$0, %r9d
	movl	$0, %r10d
	movl	$0, %r11d
	movl	$0, %edx
	jmp	.L12
	.cfi_endproc
.LFE14:
	.size	multiple_vars_sum, .-multiple_vars_sum
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"init"
.LC3:
	.string	"%-30s time: %.6f ms\n"
.LC4:
	.string	"cumulative"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC5:
	.string	"%-30s sum: %-12d time: %.6f ms\n"
	.section	.rodata.str1.1
.LC6:
	.string	"multiple 8 vars"
	.text
	.globl	main
	.type	main, @function
main:
.LFB15:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	leaq	vector(%rip), %rbp
	movq	%rbp, %rsi
	movl	$800000, %edi
	call	vector_init
	leaq	16(%rsp), %rbx
	movq	%rbx, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	16(%rsp), %rdx
	movq	24(%rsp), %rcx
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	call	time_elapsed_ms
	leaq	.LC2(%rip), %rsi
	leaq	.LC3(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movq	%rsp, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	%rbp, %rsi
	movl	$800000, %edi
	call	cumulative_sum
	movl	%eax, %r14d
	movq	%rbx, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	16(%rsp), %rdx
	movq	24(%rsp), %rcx
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	call	time_elapsed_ms
	leaq	.LC5(%rip), %r13
	movl	%r14d, %edx
	leaq	.LC4(%rip), %rsi
	movq	%r13, %rdi
	movl	$1, %eax
	call	printf@PLT
	movq	%rsp, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	%rbp, %rsi
	movl	$800000, %edi
	call	multiple_vars_sum
	movl	%eax, %ebp
	movq	%rbx, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	16(%rsp), %rdx
	movq	24(%rsp), %rcx
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	call	time_elapsed_ms
	movl	%ebp, %edx
	leaq	.LC6(%rip), %rsi
	movq	%r13, %rdi
	movl	$1, %eax
	call	printf@PLT
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L20
	movl	$0, %eax
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L20:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE15:
	.size	main, .-main
	.globl	vector
	.bss
	.align 32
	.type	vector, @object
	.size	vector, 3200000
vector:
	.zero	3200000
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	0
	.long	1083129856
	.align 8
.LC1:
	.long	0
	.long	1093567616
	.ident	"GCC: (GNU) 15.2.1 20251112"
	.section	.note.GNU-stack,"",@progbits
