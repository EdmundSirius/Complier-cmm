# 优化处理
## 公共子表达式消除 (Common-subexpression Elimination)
## 代码移动 (Code Motion）
## 代数运算简化 (Algebraic Simplification)

# 数组的翻译模式
temp: v1
temp[5]

temp[0]
```
t1 := v1
t2 := *t1
```
temp[1]
```
t1 := v1 + #4
t2 = *t1

t1 := #1
t2 := t1 * #4
t3 := &v1 + t2

&v1 + 4 == *(v1 + 4)
```
temp[3]
```
t1 := #3
t2 := t1 * #4
t3 := &v1 + t2
```

temp[i]
```
t1 := vi
t2 := t1 * #4
t2 = &v1 + t1
```

r: v1
i: v2
j: v3
r[0][i]
```
t1 := v2 * #4
t2 := &v1 + t1
```
r[i][j]
```
t1 := #2

t2 := v2 * t1
t3 := t2 * #4
t4 := &v1 + t2

t5 := v3 * #4
t6 := t4 + t5

&v1 + i * 2 * #4 + j * #4
```

[7][8]
int array[i][j]
array: v1
i: v2
j: v3
k: v4
array + i * 8 * 4 + j * 4
```
t2 := #8

t4 := v2 * t2
t6 := t4 * #4
t10 := &v1 + t6

t7 := v3 * #4
t11 := t10 + t7

```

[7][8][9]
int array[i][j][k]
array: v1
i: v2
j: v3
k: v4
array + i * 8 * 9 * 4 + j * 9 * 4 + k * 4
```
t2 := #8
t3 := #9

t4 := v2 * t2
t5 := t4 * t3
t6 := t5 * #4
t10 := &v1 + t6

t7 := v3 * t3
t8 := t7 * #4
t11 := t10 + t8

t9 := v4 * #4
t12 := t11 + t9
```

[7][8][9][10]
int array[i][j][k][s]
array: v1
i: v2
j: v3
k: v4
s: v5
array + i * 8 * 9 * 10 * 4 + j * 9 * 10 * 4 + k * 10 * 4 + s * 4
```
t2 := #8
t3 := #9
t4 := #10

t5 := v2 * t2
t6 := t5 * t3
t7 := t6 * t4
t8 := t7 * #4
t15 := &v1 + t8

t9 := v3 * t3
t10 := t9 * t4
t11 := t10 * #4
t16 := t15 + t11

t12 := v4 * t4
t13 := t12 * #4
t17 := t16 + t13

t14 := v5 * #4
t18 := t17 + t14
```
