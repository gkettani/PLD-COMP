.globl	main
 main: 
 	# prologue 
 	pushq %rbp # save %rbp on the stack 
 	movq %rsp, %rbp # define %rbp for the current function 
.bb0:
 	movl	$1, %eax
 	# epilogue 
 	popq %rbp # restore %rbp from the stack 
 	ret
