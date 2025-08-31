
  .data

# Initial buffer value
initial_buffer:
  .word 0x67452301 # A
  .word 0xefcdab89 # B
  .word 0x98badcfe # C
  .word 0x10325476 # D

# 64-element table constructed from the sine function.
K:
  .word 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
  .word 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501
  .word 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be
  .word 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
  .word 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa
  .word 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8
  .word 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed
  .word 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
  .word 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c
  .word 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70
  .word 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05
  .word 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
  .word 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039
  .word 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1
  .word 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
  .word 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391

# Per-round shift amounts
S:
  .word 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22
  .word 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20
  .word 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23
  .word 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21

A:
  .word   0
B:
  .word   0
C:
  .word   0
D:
  .word   0

  .text

# -----------------------------------------------------------------
# Function: md5
#
# Description:
#   Computes the MD5 hash of an input message using the MD5 algorithm.
#   The computed digest is stored in memory.
#
# Parameters:
#   a0 - Pointer to the beginning of the input message.
#   a1 - Length of the input message in bytes.
#   a2 - Pointer to a memory region where the resulting MD5 digest
#        will be stored.
# -----------------------------------------------------------------
processHash:
        addi    sp,sp,-96
        sw      s0,92(sp)
        sw      s1,88(sp)
        sw      s2,84(sp)
        sw      s3,80(sp)
        sw      s4,76(sp)
        sw      s5,72(sp)
        sw      s6,68(sp)
        mv      a4,sp
        addi    a2,sp,64
.L2:
        lbu     a5,1(a0)
        slli    a5,a5,8
        lbu     a3,2(a0)
        slli    a3,a3,16
        or      a5,a5,a3
        lbu     a3,0(a0)
        or      a5,a5,a3
        lbu     a3,3(a0)
        slli    a3,a3,24
        or      a5,a5,a3
        sw      a5,0(a4)
        addi    a0,a0,4
        addi    a4,a4,4
        bne     a4,a2,.L2
        lw      s4,A
        lw      s3,B
        lw      s2,C
        lw      s1,D
        la      a7,K
        la      a6,S
        mv      a0,s1
        mv      a1,s2
        mv      a2,s3
        mv      s5,s4
        li      t4,0
        li      t3,5
        li      t1,1
        li      a3,0
        li      t0,15
        li      t2,31
        li      s0,47
        mv      t6,sp
        li      t5,64
        j       .L7
.L12:
        xor     a5,a1,a0
        and     a5,a5,a2
        xor     a5,a5,a0
        mv      a4,a3
        j       .L4
.L5:
        bgt     a3,s0,.L6
        xor     a5,a2,a1
        xor     a5,a5,a0
        srai    s6,t3,31
        srli    s6,s6,28
        add     a4,t3,s6
        andi    a4,a4,15
        sub     a4,a4,s6
.L4:
        lw      s6,0(a7)
        add     a5,a5,s6
        slli    a4,a4,2
        add     a4,a4,t6
        lw      a4,0(a4)
        add     a5,a5,a4
        add     a5,a5,s5
        lw      s5,0(a6)
        sll     a4,a5,s5
        neg     s5,s5
        srl     a5,a5,s5
        or      a5,a4,a5
        add     a5,a5,a2
        addi    a3,a3,1
        addi    a7,a7,4
        addi    a6,a6,4
        addi    t1,t1,5
        addi    t3,t3,3
        addi    t4,t4,7
        mv      s5,a0
        beq     a3,t5,.L11
        mv      a0,a1
        mv      a1,a2
        mv      a2,a5
.L7:
        ble     a3,t0,.L12
        bgt     a3,t2,.L5
        xor     a5,a2,a1
        and     a5,a5,a0
        xor     a5,a5,a1
        srai    s6,t1,31
        srli    s6,s6,28
        add     a4,t1,s6
        andi    a4,a4,15
        sub     a4,a4,s6
        j       .L4
.L6:
        not     a5,a0
        or      a5,a5,a2
        xor     a5,a5,a1
        srai    s6,t4,31
        srli    s6,s6,28
        add     a4,t4,s6
        andi    a4,a4,15
        sub     a4,a4,s6
        j       .L4
.L11:
        add     s4,s4,a0
        sw      s4,A,a4
        add     s3,s3,a5
        sw      s3,B,a5
        add     s2,s2,a2
        sw      s2,C,a5
        add     s1,s1,a1
        sw      s1,D,a5
        lw      s0,92(sp)
        lw      s1,88(sp)
        lw      s2,84(sp)
        lw      s3,80(sp)
        lw      s4,76(sp)
        lw      s5,72(sp)
        lw      s6,68(sp)
        addi    sp,sp,96
        jr      ra
  .globl md5
md5:
        addi    sp,sp,-96
        sw      ra,92(sp)
        sw      s0,88(sp)
        sw      s1,84(sp)
        sw      s2,80(sp)
        sw      s4,72(sp)
        sw      s5,68(sp)
        sw      s6,64(sp)
        mv      s1,a0
        mv      s2,a1
        mv      s6,a2
        li      a5,1732583424
        addi    a5,a5,769
        sw      a5,A,a4
        li      a5,-271732736
        addi    a5,a5,-1143
        sw      a5,B,a4
        li      a5,-1732583424
        addi    a5,a5,-770
        sw      a5,C,a4
        li      a5,271732736
        addi    a5,a5,1142
        sw      a5,D,a4
        srli    s5,a1,6
        andi    s4,a1,63
        beq     s5,zero,.L14
        sw      s3,76(sp)
        mv      s0,a0
        slli    s3,s5,6
        add     s3,s3,a0
.L15:
        mv      a0,s0
        call    processHash
        addi    s0,s0,64
        bne     s0,s3,.L15
        lw      s3,76(sp)
.L14:
        beq     s4,zero,.L16
        slli    s5,s5,6
        add     a0,s1,s5
        mv      a5,sp
        add     a3,s4,sp
.L17:
        lbu     a4,0(a0)
        sb      a4,0(a5)
        addi    a0,a0,1
        addi    a5,a5,1
        bne     a5,a3,.L17
        li      a4,-128
        addi    a5,s4,64
        add     a5,a5,sp
        sb      a4,-64(a5)
        addi    a5,s4,1
        li      a4,63
        bgtu    a5,a4,.L18
.L24:
        add     a5,s4,sp
        addi    a4,sp,63
.L19:
        sb      zero,1(a5)
        addi    a5,a5,1
        bne     a4,a5,.L19
        slli    s2,s2,3
        li      a5,55
        bgtu    s4,a5,.L20
        sb      s2,56(sp)
        sb      zero,60(sp)
        srli    a5,s2,8
        sb      a5,57(sp)
        sb      zero,61(sp)
        srli    a5,s2,16
        sb      a5,58(sp)
        sb      zero,62(sp)
        srli    s2,s2,24
        sb      s2,59(sp)
        sb      zero,63(sp)
        mv      a0,sp
        call    processHash
.L23:
        mv      a2,s6
        li      a5,0
        li      a3,32
.L21:
        lw      a4,A
        srl     a4,a4,a5
        sb      a4,0(a2)
        lw      a4,B
        srl     a4,a4,a5
        sb      a4,4(a2)
        lw      a4,C
        srl     a4,a4,a5
        sb      a4,8(a2)
        lw      a4,D
        srl     a4,a4,a5
        sb      a4,12(a2)
        addi    a5,a5,8
        addi    a2,a2,1
        bne     a5,a3,.L21
        lw      ra,92(sp)
        lw      s0,88(sp)
        lw      s1,84(sp)
        lw      s2,80(sp)
        lw      s4,72(sp)
        lw      s5,68(sp)
        lw      s6,64(sp)
        addi    sp,sp,96
        jr      ra
.L18:
        slli    s2,s2,3
.L20:
        mv      a0,sp
        call    processHash
        mv      a0,sp
        addi    a5,sp,56
.L22:
        sb      zero,0(a0)
        addi    a0,a0,1
        bne     a0,a5,.L22
        sb      s2,56(sp)
        sb      zero,60(sp)
        srli    a5,s2,8
        sb      a5,57(sp)
        sb      zero,61(sp)
        srli    a5,s2,16
        sb      a5,58(sp)
        sb      zero,62(sp)
        srli    s2,s2,24
        sb      s2,59(sp)
        sb      zero,63(sp)
        mv      a0,sp
        call    processHash
        j       .L23
.L16:
        li      a4,-128
        addi    a5,s4,64
        add     a5,a5,sp
        sb      a4,-64(a5)
        j       .L24

# -----------------------------------------------------------------
# Function: print_message_digest
#
# Description:
#   Prints the 16-byte MD5 message digest in a human-readable hexadecimal format.
#   Each byte of the digest is converted to its two-digit hexadecimal representation,
#   resulting in a 32-character string that represents the hash.
#
# Parameters:
#   a0 - Pointer to the MD5 digest.
# -----------------------------------------------------------------
  .globl print_message_digest
print_message_digest:
  li t0, 0          # i from 0 to 15
  mv t4, a0
  li a0, 11         # ecall id
print_loop:
  add t1, t4, t0    # t1 = (a0 + i)
  lbu   t1, 0(t1)   # t1 = *(a0 + i)
  addi t0, t0, 1
  li t2, 10
  srli t3, t1, 4
  blt t3, t2, print_char1
  addi t3, t3, 39
print_char1:
  addi t3, t3, 48
  mv a1, t3
  ecall
  andi t3, t1, 0xF
  blt t3, t2, print_char2
  addi t3, t3, 39
print_char2:
  addi t3, t3, 48
  mv a1, t3
  ecall
  li t1, 16
  blt t0, t1, print_loop
  ret

  