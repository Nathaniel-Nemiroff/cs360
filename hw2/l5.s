	.global main, mymain, myprintf
main:
	pushl %ebp
	movl %esp, %ebp	

	pushl $FMT
	call myprintf
	addl $4, %esp

	pushl b
	pushl a
	pushl $fmt

	call myprintf
	addl $12, %esp

	subl $16, %esp
	#movl 16(%ebp), %eax
	#movl %eax, 8(%esp)
	#movl 12(%ebp), %eax
	#movl %eax, 4(%esp)
	#movl  8(%ebp), %eax
	#movl %eax,  (%esp)

	push 16(%ebp)
	push 12(%ebp)
	push  8(%ebp)
	call mymain

	addl $16, %esp

	movl %ebp, %esp
	popl %ebp
	ret

	.data
FMT:	.asciz "main() in assembly call mymain() in C\n"
a:	.long 1234
b:	.long 5678
fmt:	.asciz "a=%d b=%d\n"
