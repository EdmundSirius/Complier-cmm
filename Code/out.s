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
# FUNCTION squaRever :

squaRever:
	addi $sp, $sp, -152
# PARAM v1
# v2 := #0
	li $t1, 0
	move $t0, $t1
	sw $t0, 148($sp)
# DEC v3 12
# v4 := #0
	li $t1, 0
	move $t0, $t1
	sw $t0, 132($sp)
# t6 := v1
	lw $t1, 156($sp)
	move $t0, $t1
	sw $t0, 128($sp)
# t5 := t6 / #100
	lw $t1, 128($sp)
	li $t2, 100
	div $t1, $t2
	mflo $t0
	sw $t0, 124($sp)
# t8 := &v3 + #0
	la $t1, 136($sp)
	li $t2, 0
	add $t0, $t1, $t2
	sw $t0, 120($sp)
# *t8 := t5
	lw $t1, 124($sp)
	sw $t1, 0($t0)
# t14 := v1
	lw $t1, 156($sp)
	move $t0, $t1
	sw $t0, 116($sp)
# t12 := t14 / #10
	lw $t1, 116($sp)
	li $t2, 10
	div $t1, $t2
	mflo $t0
	sw $t0, 112($sp)
# t17 := &v3 + #0
	la $t1, 136($sp)
	li $t2, 0
	add $t0, $t1, $t2
	sw $t0, 108($sp)
# t20 := *t17
	lw $t1, 108($sp)
	lw $t0, 0($t1)
	sw $t0, 104($sp)
# t13 := #10 * t20
	li $t1, 10
	lw $t2, 104($sp)
	mul $t0, $t1, $t2
	sw $t0, 100($sp)
# t11 := t12 - t13
	lw $t1, 112($sp)
	lw $t2, 100($sp)
	sub $t0, $t1, $t2
	sw $t0, 96($sp)
# t21 := &v3 + #4
	la $t1, 136($sp)
	li $t2, 4
	add $t0, $t1, $t2
	sw $t0, 92($sp)
# *t21 := t11
	lw $t1, 96($sp)
	sw $t1, 0($t0)
# t27 := v1
	lw $t1, 156($sp)
	move $t0, $t1
	sw $t0, 88($sp)
# t30 := &v3 + #0
	la $t1, 136($sp)
	li $t2, 0
	add $t0, $t1, $t2
	sw $t0, 84($sp)
# t33 := *t30
	lw $t1, 84($sp)
	lw $t0, 0($t1)
	sw $t0, 80($sp)
# t28 := #100 * t33
	li $t1, 100
	lw $t2, 80($sp)
	mul $t0, $t1, $t2
	sw $t0, 76($sp)
# t25 := t27 - t28
	lw $t1, 88($sp)
	lw $t2, 76($sp)
	sub $t0, $t1, $t2
	sw $t0, 72($sp)
# t35 := &v3 + #4
	la $t1, 136($sp)
	li $t2, 4
	add $t0, $t1, $t2
	sw $t0, 68($sp)
# t38 := *t35
	lw $t1, 68($sp)
	lw $t0, 0($t1)
	sw $t0, 64($sp)
# t26 := #10 * t38
	li $t1, 10
	lw $t2, 64($sp)
	mul $t0, $t1, $t2
	sw $t0, 60($sp)
# t24 := t25 - t26
	lw $t1, 72($sp)
	lw $t2, 60($sp)
	sub $t0, $t1, $t2
	sw $t0, 56($sp)
# t39 := &v3 + #8
	la $t1, 136($sp)
	li $t2, 8
	add $t0, $t1, $t2
	sw $t0, 52($sp)
# *t39 := t24
	lw $t1, 56($sp)
	sw $t1, 0($t0)
# t42 := &v3 + #0
	la $t1, 136($sp)
	li $t2, 0
	add $t0, $t1, $t2
	sw $t0, 48($sp)
# t45 := *t42
	lw $t1, 48($sp)
	lw $t0, 0($t1)
	sw $t0, 44($sp)
# t46 := &v3 + #8
	la $t1, 136($sp)
	li $t2, 8
	add $t0, $t1, $t2
	sw $t0, 40($sp)
# t49 := *t46
	lw $t1, 40($sp)
	lw $t0, 0($t1)
	sw $t0, 36($sp)
# IF t45 != t49 GOTO label1
	lw $t1, 44($sp)
	lw $t2, 36($sp)
	bne $t1, $t2, label1
# GOTO label2
	j label2
# LABEL label1 :
label1:
# v2 := #0
	li $t1, 0
	move $t0, $t1
	sw $t0, 148($sp)
# GOTO label3
	j label3
# LABEL label2 :
label2:
# LABEL label4 :
label4:
# t51 := v4
	lw $t1, 132($sp)
	move $t0, $t1
	sw $t0, 32($sp)
# IF t51 < #12 GOTO label5
	lw $t1, 32($sp)
	li $t2, 12
	blt $t1, $t2, label5
# GOTO label6
	j label6
# LABEL label5 :
label5:
# t56 := v4
	lw $t1, 132($sp)
	move $t0, $t1
	sw $t0, 28($sp)
# t57 := v4
	lw $t1, 132($sp)
	move $t0, $t1
	sw $t0, 24($sp)
# t54 := t56 * t57
	lw $t1, 28($sp)
	lw $t2, 24($sp)
	mul $t0, $t1, $t2
	sw $t0, 20($sp)
# t55 := v1
	lw $t1, 156($sp)
	move $t0, $t1
	sw $t0, 16($sp)
# t53 := t54 - t55
	lw $t1, 20($sp)
	lw $t2, 16($sp)
	sub $t0, $t1, $t2
	sw $t0, 12($sp)
# IF t53 == #0 GOTO label7
	lw $t1, 12($sp)
	li $t2, 0
	beq $t1, $t2, label7
# GOTO label8
	j label8
# LABEL label7 :
label7:
# v2 := #1
	li $t1, 1
	move $t0, $t1
	sw $t0, 148($sp)
# LABEL label8 :
label8:
# t61 := v4
	lw $t1, 132($sp)
	move $t0, $t1
	sw $t0, 8($sp)
# t60 := t61 + #1
	lw $t1, 8($sp)
	li $t2, 1
	add $t0, $t1, $t2
	sw $t0, 4($sp)
# v4 := t60
	lw $t1, 4($sp)
	move $t0, $t1
	sw $t0, 132($sp)
# GOTO label4
	j label4
# LABEL label6 :
label6:
# LABEL label3 :
label3:
# t63 := v2
	lw $t1, 148($sp)
	move $t0, $t1
	sw $t0, 0($sp)
# IF t63 == #1 GOTO label9
	lw $t1, 0($sp)
	li $t2, 1
	beq $t1, $t2, label9
# GOTO label10
	j label10
# LABEL label9 :
label9:
# RETURN #1
	li $t0, 1
	move $v0, $t0
	addi $sp, $sp, 152
	jr $ra
# GOTO label11
	j label11
# LABEL label10 :
label10:
# RETURN #0
	li $t0, 0
	move $v0, $t0
	addi $sp, $sp, 152
	jr $ra
# LABEL label11 :
label11:
# FUNCTION main :

main:
	addi $sp, $sp, -636
# v5 := #100
	li $t1, 100
	move $t0, $t1
	sw $t0, 632($sp)
# LABEL label12 :
label12:
# t71 := v5
	lw $t1, 632($sp)
	move $t0, $t1
	sw $t0, 628($sp)
# IF t71 < #150 GOTO label13
	lw $t1, 628($sp)
	li $t2, 150
	blt $t1, $t2, label13
# GOTO label14
	j label14
# LABEL label13 :
label13:
# t74 := v5
	lw $t1, 632($sp)
	move $t0, $t1
	sw $t0, 624($sp)
# ARG t74
	lw $t0, 624($sp)
	addi $sp, $sp, -4
	sw $t0, 0($sp)
# t73 := CALL squaRever
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal squaRever
	lw $ra, 0($sp)
	addi $sp, $sp, 8
	move $t0, $v0
	sw $t0, 620($sp)
# IF t73 == #1 GOTO label15
	lw $t1, 620($sp)
	li $t2, 1
	beq $t1, $t2, label15
# GOTO label16
	j label16
# LABEL label15 :
label15:
# t76 := v5
	lw $t1, 632($sp)
	move $t0, $t1
	sw $t0, 616($sp)
# WRITE t76
	lw $t0, 616($sp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
# LABEL label16 :
label16:
# t78 := v5
	lw $t1, 632($sp)
	move $t0, $t1
	sw $t0, 612($sp)
# t77 := t78 + #1
	lw $t1, 612($sp)
	li $t2, 1
	add $t0, $t1, $t2
	sw $t0, 608($sp)
# v5 := t77
	lw $t1, 608($sp)
	move $t0, $t1
	sw $t0, 632($sp)
# GOTO label12
	j label12
# LABEL label14 :
label14:
# RETURN #0
	li $t0, 0
	move $v0, $t0
	addi $sp, $sp, 636
	jr $ra
