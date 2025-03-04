.data
test_input: .word 6 3 5 12

.text
main:
	add t0, x0, x0         # t0 = 0
	addi t1, x0, 4		   # t1 = 4
	la t2, test_input      # t2 = &test_input
main_loop:
	beq t0, t1, main_exit  # if t0 == t1, jump to main_exit
	slli t3, t0, 2         # t3 = t0 << 2
	add t4, t2, t3         # t4 = t2 + t3
	lw a0, 0(t4)           # a0 = *t4 (a0 = test_input[t0])

	addi sp, sp, -20       # Save t0, t1, t2, t3, t4
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw t3, 12(sp)
	sw t4, 16(sp)

	jal ra, seriesOp       # Call seriesOp

	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw t3, 12(sp)
	lw t4, 16(sp)
	addi sp, sp, 20        # Restore t0, t1, t2, t3, t4

	addi a1, a0, 0         # a1 = a0 (return value a0)
	addi a0, x0, 1
	ecall # Print Result
	addi a1, x0, ' '       # a1 = ' '
	addi a0, x0, 11
	ecall
	
	addi t0, t0, 1         # t0 = t0 + 1
	jal x0, main_loop      # loop
main_exit:  
	addi a0, x0, 10
	ecall # Exit

# int seriesOp(int n) {
#     int sign = 1;
#     int result = 0;
#     for (int i = n; i >= 1; i--) {
#         result += sign * i;
#         sign = sign * (-1);
#     }
#     return result;
# }

seriesOp: 
	mv t0, a0              # t0 = a0 (save n)
	mv t1, a0 			   # t1 = a0 (i = n)
	addi t2, x0, 1         # t2 = 1 (sign)
	addi a0, x0, 0         # a0 = 0 (result)
	ble t0, x0, zero        # if t0 <= 0, jump to zero
loop:
	mul t3, t2, t1         # t3 = t2 * t1
	add a0, a0, t3         # a0 = a0 + t3 (result += sign * i)
	neg t2, t2             # t2 = -t2 (sign = sign * (-1))
	addi t1, t1, -1        # t1 = t1 - 1 (i--)
	bgt t1, x0, loop       # if t1 >= 1, jump to loop
	ret
zero:
	ret
