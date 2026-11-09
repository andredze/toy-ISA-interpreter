li x2, 4096
li x1, 1
beq x1, x2, 6
add x1, x1, x1
add x1, x1, x1
add x1, x1, x1
add x1, x1, x1
j 2
li x8, 60
li x0, 0
syscall
