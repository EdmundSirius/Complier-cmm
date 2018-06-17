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
FUNCTION mod :

mod:
	subu $sp, $sp, 0
PARAM v1
PARAM v2
t3 := v1
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t7 := v1
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t8 := v2
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t5 := t7 / t8
	div $t0, $t0
	mflo $t1
t6 := v2
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t4 := t5 * t6
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	mul $t3, $t1, $t2
	sw $t3, 20($fp)
t2 := t3 - t4
	sub $t0, $t0, $t0
v3 := t2
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t10 := v3
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
v4 := t10
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t12 := v3
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
RETURN t12
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
FUNCTION power :

power:
	subu $sp, $sp, 0
PARAM v5
PARAM v6
t18 := v6
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t15 := #1 + t18
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	add $t3, $t1, $t2
	sw $t3, 24($fp)
t16 := v6
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t14 := t15 - t16
	sub $t0, $t0, $t0
v7 := t14
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
LABEL label1 :
label1:
t19 := v6
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t29 := v7
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t30 := v7
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t27 := t29 - t30
	sub $t0, $t0, $t0
t25 := t27 + #90
	addi $t4, $t0, 90
t23 := t25 - #89
	addi $t0, $t0, -89
t21 := t23 + #1
	addi $t5, $t0, 1
t20 := t21 - #2
	addi $t0, $t0, -2
IF t19 > t20 GOTO label2
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	bgt $t1, $t2, label2
GOTO label3
	j label3
LABEL label2 :
label2:
t32 := v7
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t33 := v5
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t31 := t32 * t33
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	mul $t3, $t1, $t2
	sw $t3, 36($fp)
v7 := t31
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t40 := v6
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t37 := #2 * t40
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	mul $t3, $t1, $t2
	sw $t3, 40($fp)
t44 := v6
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t38 := #1 * t44
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	mul $t3, $t1, $t2
	sw $t3, 44($fp)
t35 := t37 - t38
	sub $t0, $t0, $t0
t34 := t35 - #1
	addi $t0, $t0, -1
v6 := t34
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
GOTO label1
	j label1
LABEL label3 :
label3:
t46 := v7
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
RETURN t46
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
FUNCTION getNumDigits :

getNumDigits:
	subu $sp, $sp, 0
PARAM v8
v9 := #0
	li $t1, 0
	sw $t1, 0($fp)
t49 := v8
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
IF t49 < #0 GOTO label4
	li $t10, 0
	lw $t1, 12($fp)
	lw $t2, 52($fp)
	blt $t1, $t2, label4
GOTO label5
	j label5
LABEL label4 :
label4:
RETURN #-1
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
LABEL label5 :
label5:
LABEL label6 :
label6:
t54 := v8
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
IF t54 > #0 GOTO label7
	li $t11, 0
	lw $t1, 12($fp)
	lw $t2, 56($fp)
	bgt $t1, $t2, label7
GOTO label8
	j label8
LABEL label7 :
label7:
t57 := v8
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t56 := t57 / #10
	div $t0, $t0
	mflo $t12
v8 := t56
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t60 := v9
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t59 := t60 + #2
	addi $t13, $t0, 2
v9 := t59
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t63 := v9
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t62 := t63 + #2
	addi $t14, $t0, 2
v9 := t62
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t66 := v9
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t65 := t66 - #3
	addi $t0, $t0, -3
v9 := t65
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
GOTO label6
	j label6
LABEL label8 :
label8:
t69 := v9
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
RETURN t69
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
FUNCTION isNarcissistic :

isNarcissistic:
	subu $sp, $sp, 0
PARAM v10
t76 := v10
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t73 := #1 + t76
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	add $t3, $t1, $t2
	sw $t3, 72($fp)
t72 := t73 - #1
	addi $t0, $t0, -1
ARG t72
	lw $a0, 12($fp)
t71 := CALL getNumDigits
	jal getNumDigits
	sw $v0, 12($fp)
v11 := t71
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
v12 := #0
	li $t1, 0
	sw $t1, 0($fp)
t80 := v10
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
v13 := t80
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
LABEL label9 :
label9:
t81 := v13
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
IF t81 > #0 GOTO label10
	li $t17, 0
	lw $t1, 12($fp)
	lw $t2, 80($fp)
	bgt $t1, $t2, label10
GOTO label11
	j label11
LABEL label10 :
label10:
t84 := v13
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
ARG #10
	lw $a0, 12($fp)
ARG t84
	lw $a0, 12($fp)
t83 := CALL mod
	jal mod
	sw $v0, 12($fp)
v14 := t83
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t89 := v13
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t90 := v14
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t87 := t89 - t90
	sub $t0, $t0, $t0
t86 := t87 / #10
	div $t0, $t0
	mflo $t18
v13 := t86
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t92 := v12
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t94 := v14
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t95 := v11
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
ARG t95
	lw $a0, 12($fp)
ARG t94
	lw $a0, 12($fp)
t93 := CALL power
	jal power
	sw $v0, 12($fp)
t91 := t92 + t93
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	add $t3, $t1, $t2
	sw $t3, 88($fp)
v12 := t91
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
GOTO label9
	j label9
LABEL label11 :
label11:
t96 := v12
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t97 := v10
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
IF t96 == t97 GOTO label12
	lw $t1, 12($fp)
	lw $t2, 12($fp)
	beq $t1, $t2, label12
GOTO label13
	j label13
LABEL label12 :
label12:
RETURN #1
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
GOTO label14
	j label14
LABEL label13 :
label13:
RETURN #0
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
LABEL label14 :
label14:
FUNCTION main :

main:
	subu $sp, $sp, 0
v15 := #0
	li $t1, 0
	sw $t1, 0($fp)
v16 := #300
	li $t1, 300
	sw $t1, 0($fp)
LABEL label15 :
label15:
t106 := v16
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
IF t106 < #500 GOTO label16
	li $t22, 500
	lw $t1, 12($fp)
	lw $t2, 100($fp)
	blt $t1, $t2, label16
GOTO label17
	j label17
LABEL label16 :
label16:
t109 := v16
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
ARG t109
	lw $a0, 12($fp)
t108 := CALL isNarcissistic
	jal isNarcissistic
	sw $v0, 12($fp)
IF t108 == #1 GOTO label18
	li $t23, 1
	lw $t1, 12($fp)
	lw $t2, 104($fp)
	beq $t1, $t2, label18
GOTO label19
	j label19
LABEL label18 :
label18:
t111 := v16
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
WRITE t111
	lw $a0, 12($fp)
	lw $a0, 12($fp)
	jal write
t113 := v15
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t112 := t113 + #1
	addi $t24, $t0, 1
v15 := t112
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
LABEL label19 :
label19:
t116 := v16
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
t115 := t116 + #1
	addi $t25, $t0, 1
v16 := t115
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
GOTO label15
	j label15
LABEL label17 :
label17:
t118 := v15
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
WRITE t118
	lw $a0, 12($fp)
	lw $a0, 12($fp)
	jal write
t120 := v15
	lw $t1, 0($fp)
	move $t0, $t1
	sw $t0, 0($fp)
RETURN t120
	sw $t1, 12($fp)
	lw $v0, 12($fp)
	move $sp, $fp
	lw $ra, -4($sp)
	lw $fp, -8($sp)
	addiu $sp, $sp, 0
	jr $ra
