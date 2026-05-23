	.file	"task2.c"
	.text
	.p2align 4
	.globl	time_elapsed_ms
	.type	time_elapsed_ms, @function
time_elapsed_ms:
.LFB11:
	.cfi_startproc
	vxorps	%xmm1, %xmm1, %xmm1
	subq	%rdi, %rdx
	subq	%rsi, %rcx
	vcvtsi2sdq	%rdx, %xmm1, %xmm0
	vmulsd	.LC0(%rip), %xmm0, %xmm0
	vcvtsi2sdq	%rcx, %xmm1, %xmm1
	vdivsd	.LC1(%rip), %xmm1, %xmm1
	vaddsd	%xmm1, %xmm0, %xmm0
	ret
	.cfi_endproc
.LFE11:
	.size	time_elapsed_ms, .-time_elapsed_ms
	.p2align 4
	.globl	vector_init
	.type	vector_init, @function
vector_init:
.LFB12:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L13
	leal	-1(%rdi), %eax
	cmpl	$6, %eax
	jbe	.L9
	movl	%edi, %edx
	movl	$8, %ecx
	vpcmpeqd	%ymm3, %ymm3, %ymm3
	movq	%rsi, %rax
	shrl	$3, %edx
	vpsrld	$31, %ymm3, %ymm3
	vmovd	%ecx, %xmm2
	vmovdqa	.LC2(%rip), %ymm1
	salq	$5, %rdx
	vpbroadcastd	%xmm2, %ymm2
	addq	%rsi, %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L6:
	vpslld	$1, %ymm1, %ymm0
	addq	$32, %rax
	vpaddd	%ymm2, %ymm1, %ymm1
	vpaddd	%ymm3, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L6
	movl	%edi, %edx
	andl	$-8, %edx
	testb	$7, %dil
	je	.L14
	vzeroupper
.L5:
	movslq	%edx, %rax
	leal	1(%rdx,%rdx), %edx
	.p2align 4
	.p2align 4
	.p2align 3
.L8:
	movl	%edx, (%rsi,%rax,4)
	addq	$1, %rax
	addl	$2, %edx
	cmpl	%eax, %edi
	jg	.L8
.L13:
	ret
	.p2align 4,,10
	.p2align 3
.L14:
	vzeroupper
	ret
.L9:
	xorl	%edx, %edx
	jmp	.L5
	.cfi_endproc
.LFE12:
	.size	vector_init, .-vector_init
	.p2align 4
	.globl	cumulative_sum
	.type	cumulative_sum, @function
cumulative_sum:
.LFB13:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L22
	leal	-1(%rdi), %eax
	cmpl	$6, %eax
	jbe	.L23
	movl	%edi, %edx
	movq	%rsi, %rax
	vpxor	%xmm1, %xmm1, %xmm1
	shrl	$3, %edx
	salq	$5, %rdx
	addq	%rsi, %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L18:
	vpaddd	(%rax), %ymm1, %ymm1
	addq	$32, %rax
	cmpq	%rdx, %rax
	jne	.L18
	vextracti128	$0x1, %ymm1, %xmm0
	movl	%edi, %eax
	vpaddd	%xmm1, %xmm0, %xmm0
	andl	$-8, %eax
	vpsrldq	$8, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %edx
	testb	$7, %dil
	je	.L27
	vzeroupper
.L17:
	cltq
	.p2align 4
	.p2align 4
	.p2align 3
.L21:
	addl	(%rsi,%rax,4), %edx
	addq	$1, %rax
	cmpl	%eax, %edi
	jg	.L21
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L22:
	xorl	%edx, %edx
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L27:
	movl	%edx, %eax
	vzeroupper
	ret
.L23:
	xorl	%eax, %eax
	xorl	%edx, %edx
	jmp	.L17
	.cfi_endproc
.LFE13:
	.size	cumulative_sum, .-cumulative_sum
	.p2align 4
	.globl	multiple_vars_sum
	.type	multiple_vars_sum, @function
multiple_vars_sum:
.LFB14:
	.cfi_startproc
	testl	%edi, %edi
	jle	.L31
	subl	$1, %edi
	xorl	%eax, %eax
	vpxor	%xmm0, %xmm0, %xmm0
	shrl	$3, %edi
	leal	1(%rdi), %ecx
	.p2align 5
	.p2align 4
	.p2align 3
.L30:
	movq	%rax, %rdx
	addq	$1, %rax
	salq	$5, %rdx
	vpaddd	(%rsi,%rdx), %ymm0, %ymm0
	cmpl	%ecx, %eax
	jb	.L30
	vextracti128	$0x1, %ymm0, %xmm1
	vpaddd	%xmm0, %xmm1, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %eax
	vzeroupper
	ret
	.p2align 4,,10
	.p2align 3
.L31:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE14:
	.size	multiple_vars_sum, .-multiple_vars_sum
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC5:
	.string	"init"
.LC6:
	.string	"%-30s time: %.6f ms\n"
.LC7:
	.string	"cumulative"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC8:
	.string	"%-30s sum: %-12d time: %.6f ms\n"
	.section	.rodata.str1.1
.LC9:
	.string	"multiple 8 vars"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB15:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	movl	$1, %edi
	pushq	-8(%r10)
	pushq	%rbp
	movq	%rsp, %rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x60,0x6
	.cfi_escape 0x10,0xe,0x2,0x76,0x78
	.cfi_escape 0x10,0xd,0x2,0x76,0x70
	.cfi_escape 0x10,0xc,0x2,0x76,0x68
	pushq	%rbx
	.cfi_escape 0x10,0x3,0x2,0x76,0x58
	leaq	vector(%rip), %rbx
	leaq	3200000(%rbx), %r12
	subq	$104, %rsp
	movq	%fs:40, %rsi
	movq	%rsi, -56(%rbp)
	leaq	-96(%rbp), %rsi
	call	clock_gettime@PLT
	movl	$8, %edx
	vpcmpeqd	%ymm3, %ymm3, %ymm3
	movq	%rbx, %rax
	vmovd	%edx, %xmm2
	vmovdqa	.LC2(%rip), %ymm1
	vpsrld	$31, %ymm3, %ymm3
	vpbroadcastd	%xmm2, %ymm2
	.p2align 5
	.p2align 4
	.p2align 3
.L34:
	vpslld	$1, %ymm1, %ymm0
	addq	$32, %rax
	vpaddd	%ymm2, %ymm1, %ymm1
	vpaddd	%ymm3, %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%r12, %rax
	jne	.L34
	leaq	-80(%rbp), %rsi
	movl	$1, %edi
	vzeroupper
	call	clock_gettime@PLT
	vxorpd	%xmm5, %xmm5, %xmm5
	movq	-80(%rbp), %rax
	subq	-96(%rbp), %rax
	vcvtsi2sdq	%rax, %xmm5, %xmm0
	vmulsd	.LC0(%rip), %xmm0, %xmm0
	movq	-72(%rbp), %rax
	leaq	.LC5(%rip), %rsi
	subq	-88(%rbp), %rax
	leaq	.LC6(%rip), %rdi
	vcvtsi2sdq	%rax, %xmm5, %xmm1
	vdivsd	.LC1(%rip), %xmm1, %xmm1
	movl	$1, %eax
	vaddsd	%xmm1, %xmm0, %xmm0
	call	printf@PLT
	leaq	-96(%rbp), %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	%r12, %rax
	vpxor	%xmm1, %xmm1, %xmm1
	subq	%rbx, %rax
	testb	$32, %al
	je	.L35
	vmovdqa	(%rbx), %ymm1
	addq	$32, %rbx
	cmpq	%rbx, %r12
	je	.L44
	.p2align 5
	.p2align 4
	.p2align 3
.L35:
	vpaddd	(%rbx), %ymm1, %ymm1
	addq	$64, %rbx
	vpaddd	-32(%rbx), %ymm1, %ymm1
	cmpq	%rbx, %r12
	jne	.L35
.L44:
	vmovdqa	%ymm1, -144(%rbp)
	leaq	-80(%rbp), %rsi
	movl	$1, %edi
	vzeroupper
	call	clock_gettime@PLT
	vxorpd	%xmm4, %xmm4, %xmm4
	movq	-80(%rbp), %rax
	subq	-96(%rbp), %rax
	vmovdqa	-144(%rbp), %ymm1
	leaq	.LC7(%rip), %rsi
	leaq	.LC8(%rip), %rdi
	vcvtsi2sdq	%rax, %xmm4, %xmm0
	vmulsd	.LC0(%rip), %xmm0, %xmm0
	movq	-72(%rbp), %rax
	subq	-88(%rbp), %rax
	vcvtsi2sdq	%rax, %xmm4, %xmm2
	vdivsd	.LC1(%rip), %xmm2, %xmm2
	movl	$1, %eax
	vaddsd	%xmm2, %xmm0, %xmm2
	vextracti128	$0x1, %ymm1, %xmm0
	vpaddd	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %edx
	vmovapd	%xmm2, %xmm0
	vzeroupper
	call	printf@PLT
	leaq	-96(%rbp), %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	leaq	vector(%rip), %rsi
	movl	$800000, %edi
	call	multiple_vars_sum
	leaq	-80(%rbp), %rsi
	movl	$1, %edi
	movl	%eax, %ebx
	call	clock_gettime@PLT
	vxorpd	%xmm4, %xmm4, %xmm4
	movq	-80(%rbp), %rax
	movl	%ebx, %edx
	subq	-96(%rbp), %rax
	leaq	.LC9(%rip), %rsi
	leaq	.LC8(%rip), %rdi
	vcvtsi2sdq	%rax, %xmm4, %xmm0
	vmulsd	.LC0(%rip), %xmm0, %xmm0
	movq	-72(%rbp), %rax
	subq	-88(%rbp), %rax
	vcvtsi2sdq	%rax, %xmm4, %xmm1
	vdivsd	.LC1(%rip), %xmm1, %xmm1
	movl	$1, %eax
	vaddsd	%xmm1, %xmm0, %xmm0
	call	printf@PLT
	movq	-56(%rbp), %rax
	subq	%fs:40, %rax
	jne	.L46
	addq	$104, %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
.L46:
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
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC2:
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.ident	"GCC: (GNU) 15.2.1 20260209"
	.section	.note.GNU-stack,"",@progbits
