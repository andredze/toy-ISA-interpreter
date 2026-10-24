    li x8, 0
    syscall

    li x8, 1
    movn x4, x0, x8

    li x0, 0
    li x5, 0
    li x6, 1

    beq x4, x0, 6

    add x7, x5, x6
    addi x5, x6, 0
    addi x6, x7, 0

    addi x4, x4, -1
    j 7

    li x8, 1
    movn x1, x5, x8
    syscall

    li x8, 60
    li x0, 0
    syscall