.globl greatest_common_divider main

.data
n: .word 12 3 # You can change this for extra test cases - TAs will use more test cases during Lab4 check!
txt1: .asciiz "greatest_common_divider("
txt2: .asciiz ", "
txt3: .asciiz ") = "

.text
main:
    # Note for ecall:
	# a0==4  --> puts(a1)
	# a0==1  --> printf("%" PRId32 "", a1)
	# a0==11 --> putchar(a1)
	# a0==10 --> exit()
    li a0, 4
    la a1, txt1
    ecall
    li a0, 1
    la t0, n
    lw a1, 0(t0)
    ecall
    li a0, 4
    la a1, txt2
    ecall
	li a0, 1
    la t0, n
    lw a1, 4(t0)
    ecall
	li a0, 4
    la a1, txt3
    ecall

    lw a0, 0(t0)
	lw a1, 4(t0)
    jal ra, greatest_common_divider
    
	add t0, a0, x0
    li a0, 1
	add a1, t0, x0
    ecall
	li a0, 11
	li a1, '\n'
	ecall
    li a0, 10
    ecall


# int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
greatest_common_divider:
    # PROLOGUE BEGIN
    addi sp, sp, -12
    sw ra, 8(sp)
    # PROLOGUE END
    beqz a1, end
    rem t0, a0, a1
    mv a0, a1
    mv a1, t0
    jal ra, greatest_common_divider
end:
    # EPILOGUE BEGIN
    lw ra, 8(sp)
    addi sp, sp, 12
    # EPILOGUE END
	ret
