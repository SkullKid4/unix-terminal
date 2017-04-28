
#full header in syscall_link.h
#saves registers and calls correct system call

.globl syscall_link

syscall_link:

	pushl %ebx		#save registers
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi


	#prepare for sytem call
	cmpl $0, %eax
	jle not_valid
	cmpl $11, %eax
	jge not_valid

	pushl %edx		#push argumetns
	pushl %ecx
	pushl %ebx

	subl $1, %eax	#sub 1 becasue jump table is zero indexed

	call *jump_table(, %eax, 4)

	popl %ebx		#restore stack
	popl %ecx
	popl %edx

	popl %edi		#retosre registers
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	iret


not_valid:				#return -1 on error
	movl $-1, %eax
	popl %edi		#retosre registers
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	iret	

jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


#full header in syscall_link.h
#sets up stack for user space

.globl jump_user_space
.globl HALTED
jump_user_space:
	 pushl %ebp
  	 movl %esp, %ebp
	 
	 pushl $0x2B # <== USER_DS
	 movl  $0x83FFFFC, %eax #final memory address of file
	 pushl %eax 
	 pushfl
     popl %edx
     orl $0x200, %edx	#<== push flags then or with 0x200 to set the 10 index bit to enable interupts
     pushl %edx
	 pushl $0x23 # <== USER_CS
	 movl 8(%ebp), %eax
	 pushl %eax  # <== start of user file
	 iret

HALTED:
	 leave
	 ret


.globl rtc_linkage
#save all registers and pop all registers before calling interrupt handlers
rtc_linkage:
	pushal
	call rtc_handler
	popal
	iret

.globl keyboard_linkage
keyboard_linkage:
	pushal
	call keyboard_handler
	popal
	iret
	
.globl pit_linkage
pit_linkage:
	pushal
	#call scheduler_handler
	call pit_handler
	popal
	iret





