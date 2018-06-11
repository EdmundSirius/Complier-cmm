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
	move $a0, $t0	move $t1, $t2
	li $t3, 1
	beq $t1, $t3, label1
	j label2
label1 :
	move $t4, $t2
	move $v0, reg(t4)
	jr $ra
	j label3
label2 :
	move $t5, $t2
	move $t6, $t2
Hello
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal fact
	move $t0 , $v0
Hello
	move $v0, reg(t6)
	jr $ra
label3 :
main:
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $t7, $v0
	move $t8, $t7
	move $t9, $t8
	li $t10, 1
	bgt $t9, $t10, label4
	j label5
label4 :
	move $t11, $t8
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal fact
	move $t0 , $v0
	move $t12, $t13
	j label6
label5 :
	li $t14, 1
label6 :
	move $t15, $t12
	la $t0, t18
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $v0, $0
	jr $ra
