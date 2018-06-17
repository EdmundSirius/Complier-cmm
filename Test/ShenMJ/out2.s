.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

fact:
	addiu $sp, $sp, -60
	sw $ra, 56($sp)
	sw $fp, 52($sp)
	move $fp, $sp
	sw $a0, 60($fp)
	lw $t1, 60($fp)
	sw $t1, 16($fp)
	li $t1, 1
	sw $t1, 20($fp)
	lw $t1, 16($fp)
	lw $t2, 20($fp)
	beq $t1, $t2, L1
	b L2
L1:
	lw $t1, 60($fp)
	sw $t1, 24($fp)
	lw $v0, 24($fp)
	b fact_epilogue
	b L3
L2:
	lw $t1, 60($fp)
	sw $t1, 28($fp)
	lw $t1, 60($fp)
	sw $t1, 32($fp)
	li $t1, 1
	sw $t1, 36($fp)
	lw $t1, 32($fp)
	lw $t2, 36($fp)
	subu $t3, $t1, $t2
	sw $t3, 40($fp)
	lw $a0, 40($fp)
	jal fact
	sw $v0, 44($fp)
	lw $t1, 28($fp)
	lw $t2, 44($fp)
	mul $t3, $t1, $t2
	sw $t3, 48($fp)
	lw $v0, 48($fp)
	b fact_epilogue
L3:
fact_epilogue:
	move $sp, $fp
	lw $ra, 56($sp)
	lw $fp, 52($sp)
	addiu $sp, $sp, 60
	jr $ra

main:
	addiu $sp, $sp, -64
	sw $ra, 60($sp)
	sw $fp, 56($sp)
	move $fp, $sp
	jal read
	sw $v0, 16($fp)
	lw $t1, 16($fp)
	sw $t1, 20($fp)
	lw $t1, 20($fp)
	sw $t1, 24($fp)
	li $t1, 1
	sw $t1, 28($fp)
	lw $t1, 24($fp)
	lw $t2, 28($fp)
	bgt $t1, $t2, L4
	b L5
L4:
	lw $t1, 20($fp)
	sw $t1, 32($fp)
	lw $a0, 32($fp)
	jal fact
	sw $v0, 36($fp)
	lw $t1, 36($fp)
	sw $t1, 40($fp)
	b L6
L5:
	li $t1, 1
	sw $t1, 44($fp)
	lw $t1, 44($fp)
	sw $t1, 40($fp)
L6:
	lw $t1, 40($fp)
	sw $t1, 48($fp)
	lw $a0, 48($fp)
	jal write
	li $t1, 0
	sw $t1, 52($fp)
	lw $v0, 52($fp)
	b main_epilogue
main_epilogue:
	move $sp, $fp
	lw $ra, 60($sp)
	lw $fp, 56($sp)
	addiu $sp, $sp, 64
	jr $ra
