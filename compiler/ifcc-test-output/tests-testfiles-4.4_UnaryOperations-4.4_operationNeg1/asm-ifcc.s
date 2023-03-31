.globl	main
 main: 
 	# prologue 
 	pushq %rbp # save %rbp on the stack 
 	movq %rsp, %rbp # define %rbp for the current function 
.bb0:
 	movl	$5, -4(%rbp)
	movl	-4(%rbp), %eax
    negl	%eax
 	# epilogue 
 	popq %rbp # restore %rbp from the stack 
 	ret
