# 词法分析与语法分析

## 程序功能(必做+选做1.1)
### 数组访问错误处理

对于实验说明中，
> 输入文件的同一行中保证不出现多个错误，你的程序需要将这些错误全部报告出来，每一条错误提示信息在输出中单独占一行。

程序对数组访问错误的处理方式和讲义中稍有不同。

```
int main()
{
	float a[10][2];
	int i;
	a[5,3] = 1.5;
	if (a[1][2] == 0) i = 1 else i = 0;
}
```
程序将报告三个错误，其中第4行有两个错误，一个是“]”缺失，一个是语句不合理。

```
Error type B at Line 4: Missing "]".
Error type B at Line 4: Unvalid statement.
Error type B at Line 5: Missing ";".
```
这是为错误恢复提供便利，如果单纯将错误认定为“]”缺失，程序将无法处理如下输入文件，因为不能确保任何“]”缺失都可以有相应的恢复策略。

```
int main()
{
    int a[10];
    int b[10];
    int c;
    c = b[a[1];
}
```
输出：

```
Error type B at Line 6: Missing "]".
```
### C--语言和C语言的文法差异处理
对于C--语言中没有的标识符`for`,`double`等进行了恢复处理，对输入文件中类似的标识符进行识别，如下，

```
int main()
{
    int float = 1;
    float 1err = 1.0;
}
```
输出：

```
Error type B at Line 3: Unvalid name of variable.
Error type B at Line 4: Unvalid identifier.
```
## 实现思路
### 文件组织
```
Code目录下：
.
├── Makefile
├── lexical.l
├── main.c
├── parse.c
├── parse.h
└── syntax.y
```
### 数据结构
定义`TreeNode`类型结构体，建立语法多叉树。

```
typedef struct TreeNode {
    char name[32];	//词法单元
    char text[32];	//词素
    int lineno;		//行号
    int childsum;	//子节点数目
    struct TreeNode* child[MAX_CHILD_NUM];
} Node;
```
主要功能函数如下，

```
Node* createNode(char*, char*);
void insertNode(int, Node*, ...);
void printNode(char*, char*);
void printTree(Node *, int);
```
### 实现过程

<!--```flow
a=>operation: 检验词法分析程序
b=>operation: 恢复解决文法歧义
c=>operation: 添加文法的产生式
d=>operation: 构建并打印语法树

a->b->c->d
```
-->

1. **检验词法分析程序**
根据C--文法构造相应的正则表达式，并额外定义了常见错误形式的正则表达式(主要针对C--和C语言的文法差异)，充分利用了正则表达式的匹配规则，权衡了正确性了代码的可读性，如下，
	* 不合法的十进制
	* 不合法的八进制
	* 不合法的十六进制
	* 不合法的单精度浮点数
	* +=，-=，/\*，\*/，//

1. **添加文法的产生式**
根据C--语言文法添加产生式，并通过`%left`、`%right`和`%nonassoc`以及它们的顺序对终运算符的优先级(precedence)以及结合性(associativity)
进行规定。

1. **恢复解决文法歧义**
悬空else问题的解决方案参照讲义中增加`LOWER_THAN_ELSE`算符完成，其他二义文法均使用词法单元error进行错误恢复处理。

2. **构建并打印语法树**
先序遍历多叉树，按照讲义要求递归打印语法树节点。

### 代码风格
* 把功能的底层实现封装到函数中，增加代码的可读性。

* 开启宏定义`#define PRINT_DEBUG`进入调试模式，可以额外打印结点的词法单元类型，并在语义恢复之后输出语法树(即使存在语法错误)。

## 写在最后
和室友聊天说起编译原理实验的讲义，我们都觉得相对PA和OS，这本指导手册的可读性更高一些。
坦白的说，之前的两个实验使我对稍具规模的实验任务产生了自然而然的畏惧感。
不过欣喜的是，我感受到了实验任务和理论课程的直接联系非常紧密，这使我在完成实验一的过程中有不小的成就感。

最后非常感谢助教学长和许畅老师对我的问题详尽的解答！


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


6, 7
array[1][2]
&v1 + 1 * 7 * 4 + 2 * 4
(1 * 7 + 2) * 4

6, 7, 8
array[1][2][3]
&v1 + 1 * 7 * 8 * 4 + 2 * 8 * 4 + 3 * 4


((1 * 7 + 2) * 8 + 3) * 4
