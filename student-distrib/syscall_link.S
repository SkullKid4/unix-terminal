.globl syscall_link

syscall_link:

	pushl %ebx
	pushl %ecx
	pushl %edx
	#pushl %esp
	#pushl %ebp
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
	#popl %ebp
	#popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	iret


not_valid:
	movl $-1, %eax
	ret 			

jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

.globl jump_user_space
jump_user_space:
	 pushl %ebp
  	 movl %esp, %ebp
	 
	 pushl $0x2B # //0x2B 0x23
	 movl  $0x83FFFFC, %eax# final memory address of file
	 pushl %eax #
	 pushfl
     popl %edx
     orl $0x200, %edx
     pushl %edx
	 pushl $0x23 # //0x23 0x1B
	 movl 8(%ebp), %eax
	 pushl %eax
	 iret

HALTED:
	 leave
	 ret






	 # stack setup for iret
	 # ss <---user ss (user data segmant)
	 # esp <--- user esp (user start of stack)
	 # eflags
	 # cs <--- user
	 # eip
	 # iret



# jump_user_space:
# 	 pushl %ebp
#   	 movl %esp, %ebp
#   	 pushl %ebx
# 	 mov $0x23, %ax # // 0x2B 0x23
# 	 mov %ax, %ds#
# 	 mov %ax, %es#
# 	 mov %ax, %fs#
# 	 mov %ax, %gs#
# 	 movl  $0x83FFFFC, %eax# final memory address of file
# 	 pushl $0x23 # //0x2B 0x23
# 	 pushl %eax #
# 	 pushfl		#
# 	 popl %edx	#
# 	# orl $0x200, %edx # //enable interrupts and push flags
# 	# pushl %edx #
# 	 pushl $0x1B # //0x23 0x1B
# 	 movl 8(%ebp), %eax
# 	 pushl %eax
# 	 iret
# 	 HALTED:
# 	 leave
# 	 ret


