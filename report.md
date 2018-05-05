# 语义分析

## 程序功能(必做+选做2.3)


## 实现思路
### 数据结构
#### 符号表的设计
符号表使用Hash表，将基本类型、数组类型、结构类型、函数类型变量存储在一个统一的符号表中，提高插入和查找的效率。

符号表：

```
struct SymbolTable {
	char name[32];
	Type type;
	int lineno;
	bool occupied;
} symboltable[0x4000];
```

变量类型：

```
typedef struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
        Function function;
    } u;
} Type_;
```
其中，枚举类型变量`BASIC`，`ARRAY`，`STRUCTURE`，`FUNCTION`变量类型，在`getNodeType`和`handleExp`中作为返回类型的标识。实现假设每种类型的变量个数不会超过10个，因此使用了`类型 * 10 + 序号`作为标识。

函数类型：

```
typedef struct Function
{
    enum { INT, FLOAT } returnvalue;
    int argsum;
    int argbasic[8];
} Function;
```

结构体类型：

```
typedef struct FieldList_
{
    char name[32];
    Type type;
    FieldList tail;
} FieldList_;
```

#### 结构体的管理
使用链表结构存储结构体，包括结构体名称和其中的成员变量。

```
typedef struct StructNode_ {
    int no;
    char name[32];
    FieldList children;
    StructNode next;
} StructNode_;
```

#### 文法属性的处理
重新遍历实验一中建立的树，对文件进行语义分析。使用地址传递函数参数和返回值相结合的方式对文法的继承属性和综合属性进行处理。

```
// ExtDefList -> ExtDef ExtDefList | ϵ
void handleExtDefList(Node* root) {
    if (Childsum == 0) return;
    // Iteration is better than recursion.
    while (root != NULL) {
        handleExtDef(Child(0));
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleExtDefList bingo\n");
    #endif
}
```

#### 全局变量的使用
由于函数传递参数的缺陷，使用全局变量进行相关辅助。

* **`PARA`** 用于标识所在位置是否属于函数的参数定义。
* **`IN_STRUCT`** 用于标识结构体嵌套的深度。
* **`FLAG`** 用于标识`ExtDef`的产生式的类型。


### 实现过程

1. **设计数据结构，规划整体实现流程**	

1. **将变量加入符号表，处理相关语义错误**

1. **进行变量声明和类型检查**

1. **构造测试用例进行调试，尽量补充遗漏的特例**


### 代码风格
优点：

* 把功能的底层实现封装到函数中，增加代码的可读性。

* 在实验一的基础上增加宏定义`#define PHASE_SEM`进入调试模式，可以打印插入符号表的相关信息。（将实验一中的宏`PRINT_DEBUG`更名为`PRINT_TRACE`。）

* 使用多个参数的宏定义，子孙节点的访问和控制，最多支持6个参数。

有待提高：

* 对指针的操作不是很熟练，相关代码实现结构性相对较差，给实现和阅读带来不便。

* 对于结构体嵌套和多维数组以及一些局部变量的处理存在硬编码。

## 写在最后

报告至此，我的心情十分复杂。实现编译器的语义分析的复杂度远超出我之前的想象。体会最深的一点是有必要在动手之前设计出合理的数据结构，不然将会带来极大的困难。设计模式是否友好奠定了整个工程的基础。大学三年，第一次在写实验的过程中感受如此深刻。多次的代码重构使我有点烦躁，但不得不承认，加深了我对理论知识的理解，提高了自己的动手能力，感觉收获很大。

--

王芷芙
151220117@smail.nju.edu.cn