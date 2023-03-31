.globl	main
 main: 
 	# prologue 
 	pushq %rbp # save %rbp on the stack 
 	movq %rsp, %rbp # define %rbp for the current function 
.bb0:
 	movl	$1, -4(%rbp)
  cmpl	$0, -4(%rbp)
  sete  %al
  movzbl	%al, %eax
 	movl	%eax, -8(%rbp)
 	movl	-8(%rbp), %eax
 	# epilogue 
 	popq %rbp # restore %rbp from the stack 
 	ret
