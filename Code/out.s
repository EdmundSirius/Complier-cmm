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
	li $t1, 0
	move $t2, $t1
	li $t3, 1
	move $t4, $t3
	li $t5, 0
	move $t6, $t5
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $t7, $v0
	move $t8, $t7
label1:
	move $t9, $t6
	move $t10, $t8
	move $t10, $t8
	blt $t9, $t10, label2
	j label3
label2:
	move $t11, $t2
	move $t12, $t4
	add $t13, $t11, $t12
	move $t14, $t13
	move $t15, $t4
	la $t0, t14
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $t16, $t4
	move $t2, $t16
	move $t17, $t14
	move $t4, $t17
	move $t18, $t6
	li $t19, 1
	add $t20, $t18, $t19
	move $t6, $t20
	j label1
label3:
	li $t21, 0
	move $v0, $t21
	jr $ra
