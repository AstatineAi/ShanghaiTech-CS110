.data
n: .word 9

.text
main:   add     t0, x0, x0 # add           t0 = x0 + x0, that is, set t0 to 0
        addi    t1, x0, 1  # add immediate t1 = x0 + 1, that is, set t1 to 1
        la      t3, n      # load address  t3 = &n
        lw      t3, 0(t3)  # load word     t3 = *t3
fib:    beq     t3, x0, finish # branch equal  t3 == x0, if n == 0, jump to label finish
        add     t2, t1, t0     # add           t2 = t1 + t0
        mv      t0, t1         # move          t0 = t1
        mv      t1, t2         # move          t1 = t2
        addi    t3, t3, -1     # add immediate t3 = t3 - 1
        j       fib            # jump          goto label fib

# In the finish label, we are going to 
# print an integer by system call instruction ECALL.
finish: addi    a0, x0, 1       # System call number for print integer
        addi    a1, t0, 0       # integer to print
        ecall                   # print integer ecall
        addi    a0, x0, 10
        ecall                   # terminate ecall

