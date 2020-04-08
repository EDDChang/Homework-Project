.globl __start

.rodata
    msg_input: .string "Enter a number: "
    msg_result: .string "The result is: "
    newline: .string "\n"

.text

__start:
  # prints msg_input
    li a0, 4
    la a1, msg_input
    ecall
  # read from standard input
    li a0, 5
    ecall

################################################################################ 
# write your recursive code here, input is in a0, store the result(integer type) to t0
# ra:return adress
# sp:stack pointer
# t0:temporary/alternate link register
# a0:function arg/return values
# t1,t2:tempo
    jal ra,recur
    addi t0,a0,0
    jal ra,result
recur:
    addi sp,sp,-8
    sw x1,4(sp)
    sw a0,0(sp)
    addi t0,a0,-2
    bge t0,x0,L
    addi a0,x0,1
    addi sp,sp,8
    jalr x0,0(x1)
L:
    srli a0,a0,1     #a=a/2
    jal x1,recur    #call recur(n/2)
    addi t1,a0,0    #t1 = recur(n/2)
    lw a0,0(sp)     #a0 = n
    lw x1,4(sp)
    addi sp,sp,8
    slli t2,t1,1    #t2 = 2*recur(n/2)
    add a0,a0,t2
    jalr x0,0(x1)
################################################################################

result:
  # prints msg_result
    li a0, 4
    la a1, msg_result
    ecall
  # prints the result in t0
    li a0, 1
    mv a1, t0
    ecall
  # ends the program with status code 0
    li a0, 10
    ecall

