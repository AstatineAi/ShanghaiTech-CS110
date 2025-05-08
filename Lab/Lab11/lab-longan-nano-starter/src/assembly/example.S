.section .text
.globl draw
.type draw, @function
draw:
	li a0,3
	li a1,3
	li a2,63
	li a3,63
	li a4,0xF800
	addi sp,sp,-4
	sw ra,0(sp)
	jal LCD_DrawRectangle
	lw ra,0(sp)
	addi sp,sp,4
# lcd.c line 480 LCD_DrawRectangle
