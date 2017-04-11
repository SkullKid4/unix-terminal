.globl syscall_link

syscall_link:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi


		#prepair for sytem call
	cmpl $0, %eax
	jle not_valid
	cmpl $11, %eax
	jge not_valid

	pushl %edx
	pushl %ecx
	pushl %ebx

	subl $1, %eax

	call *jump_table(, %eax, 4)

	popl %ebx
	popl %ecx
	popl %edx

	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	ret


not_valid:
	movl $-1, %eax
	ret 			

jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
    