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

main:
	addiu $sp, $sp, -108
	sw $ra, 104($sp)
	sw $fp, 100($sp)
	move $fp, $sp
	li $t1, 0
	sw $t1, 16($fp)
	lw $t1, 16($fp)
	sw $t1, 20($fp)
	li $t1, 1
	sw $t1, 24($fp)
	lw $t1, 24($fp)
	sw $t1, 28($fp)
	li $t1, 0
	sw $t1, 32($fp)
	lw $t1, 32($fp)
	sw $t1, 36($fp)
	jal read
	sw $v0, 40($fp)
	lw $t1, 40($fp)
	sw $t1, 44($fp)
L1:
	lw $t1, 36($fp)
	sw $t1, 48($fp)
	lw $t1, 44($fp)
	sw $t1, 52($fp)
	lw $t1, 48($fp)
	lw $t2, 52($fp)
	blt $t1, $t2, L2
	b L3
L2:
	lw $t1, 20($fp)
	sw $t1, 56($fp)
	lw $t1, 28($fp)
	sw $t1, 60($fp)
	lw $t1, 56($fp)
	lw $t2, 60($fp)
	addu $t3, $t1, $t2
	sw $t3, 64($fp)
	lw $t1, 64($fp)
	sw $t1, 68($fp)
	lw $t1, 28($fp)
	sw $t1, 72($fp)
	lw $a0, 72($fp)
	jal write
	lw $t1, 28($fp)
	sw $t1, 76($fp)
	lw $t1, 76($fp)
	sw $t1, 20($fp)
	lw $t1, 68($fp)
	sw $t1, 80($fp)
	lw $t1, 80($fp)
	sw $t1, 28($fp)
	lw $t1, 36($fp)
	sw $t1, 84($fp)
	li $t1, 1
	sw $t1, 88($fp)
	lw $t1, 84($fp)
	lw $t2, 88($fp)
	addu $t3, $t1, $t2
	sw $t3, 92($fp)
	lw $t1, 92($fp)
	sw $t1, 36($fp)
	b L1
L3:
	li $t1, 0
	sw $t1, 96($fp)
	lw $v0, 96($fp)
	b main_epilogue
main_epilogue:
	move $sp, $fp
	lw $ra, 104($sp)
	lw $fp, 100($sp)
	addiu $sp, $sp, 108
	jr $ra

