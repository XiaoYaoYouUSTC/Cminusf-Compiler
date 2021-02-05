# cminus-f 的语法与语义

## cminus-f 的语法

1. $`\text{program} \rightarrow \text{declaration-list}`$
2. $`\text{declaration-list} \rightarrow \text{declaration-list}\ \text{declaration}\ |\ \text{declaration}`$
3. $`\text{declaration} \rightarrow \text{var-declaration}\ |\ \text{fun-declaration}`$
4. $`\text{var-declaration}\ \rightarrow \text{type-specifier}\ \textbf{ID}\ \textbf{;}\ |\ \text{type-specifier}\ \textbf{ID}\ \textbf{[}\ \textbf{INTEGER}\ \textbf{]}\ \textbf{;}`$
5. $`\text{type-specifier} \rightarrow \textbf{int}\ |\ \textbf{float}\ |\ \textbf{void}`$
6. $`\text{fun-declaration} \rightarrow \text{type-specifier}\ \textbf{ID}\ \textbf{(}\ \text{params}\ \textbf{)}\ \text{compound-stmt}`$
7. $`\text{params} \rightarrow \text{param-list}\ |\ \textbf{void}`$
8. $`\text{param-list} \rightarrow \text{param-list}\ ,\ \text{param}\ |\ \text{param}`$
9. $`\text{param} \rightarrow \text{type-specifier}\ \textbf{ID}\ |\ \text{type-specifier}\ \textbf{ID}\ \textbf{[]}`$
10. $`\text{compound-stmt} \rightarrow \textbf{\{}\ \text{local-declarations}\ \text{statement-list} \textbf{\}}`$
11. $`\text{local-declarations} \rightarrow \text{local-declarations var-declaration}\ |\ \text{empty}`$
12. $`\text{statement-list} \rightarrow \text{statement-list}\ \text{statement}\ |\ \text{empty}`$
13. $`\begin{aligned}\text{statement} \rightarrow\ &\text{expression-stmt}\\ &|\ \text{compound-stmt}\\ &|\ \text{selection-stmt}\\ &|\ \text{iteration-stmt}\\ &|\ \text{return-stmt}\end{aligned}`$
14. $`\text{expression-stmt} \rightarrow \text{expression}\ \textbf{;}\ |\ \textbf{;}`$
15. $`\begin{aligned}\text{selection-stmt} \rightarrow\ &\textbf{if}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}\\ &|\ \textbf{if}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}\ \textbf{else}\ \text{statement}\end{aligned}`$
16. $`\text{iteration-stmt} \rightarrow \textbf{while}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}`$
17. $`\text{return-stmt} \rightarrow \textbf{return}\ \textbf{;}\ |\ \textbf{return}\ \text{expression}\ \textbf{;}`$
18. $`\text{expression} \rightarrow \text{var}\ \textbf{=}\ \text{expression}\ |\ \text{simple-expression}`$
19. $`\text{var} \rightarrow \textbf{ID}\ |\ \textbf{ID}\ \textbf{[}\ \text{expression} \textbf{]}`$
20. $`\text{simple-expression} \rightarrow \text{additive-expression}\ \text{relop}\ \text{additive-expression}\ |\ \text{additive-expression}`$
21. $`\text{relop}\ \rightarrow \textbf{<=}\ |\ \textbf{<}\ |\ \textbf{>}\ |\ \textbf{>=}\ |\ \textbf{==}\ |\ \textbf{!=}`$
22. $`\text{additive-expression} \rightarrow \text{additive-expression}\ \text{addop}\ \text{term}\ |\ \text{term}`$
23. $`\text{addop} \rightarrow \textbf{+}\ |\ \textbf{-}`$
24. $`\text{term} \rightarrow \text{term}\ \text{mulop}\ \text{factor}\ |\ \text{factor}`$
25. $`\text{mulop} \rightarrow \textbf{*}\ |\ \textbf{/}`$
26. $`\text{factor} \rightarrow \textbf{(}\ \text{expression}\ \textbf{)}\ |\ \text{var}\ |\ \text{call}\ |\ \text{integer}\ |\ \text{float}`$
27. $`\text{integer} \rightarrow \textbf{INTEGER}`$
28. $`\text{float} \rightarrow \textbf{FLOATPOINT}`$
29. $`\text{call} \rightarrow \textbf{ID}\ \textbf{(}\ \text{args} \textbf{)}`$
30. $`\text{args} \rightarrow \text{arg-list}\ |\ \text{empty}`$
31. $`\text{arg-list} \rightarrow \text{arg-list}\ \textbf{,}\ \text{expression}\ |\ \text{expression}`$


## cminus-f 的语义

在上述语法规则中，我们定义了 `cminus-f` 语言的语法，接着，我们对照语法规则，给出相关的语义和解释。
在阅读前，需要理解 `cminus-f` 主要源自于 C 语言，因此它的行为都会接近 C 语言。

1. $`\text{program} \rightarrow \text{declaration-list}`$
2. $`\text{declaration-list} \rightarrow \text{declaration-list}\ \text{declaration}\ |\ \text{declaration}`$
3. $`\text{declaration} \rightarrow \text{var-declaration}\ |\ \text{fun-declaration}`$

  一个`程序`由一系列`声明`组成，声明包括了`函数声明`与`变量声明`，它们可以以任意顺序排列。

  全局变量需要初始化为全 0

  所有的变量必须在使用前先进行声明，所有的函数必须在使用前先进行定义

  一个`程序`中至少要有一个`声明`，且最后一个`声明`必须是 `void main(void)` 形式的`函数声明`。 

  因为没有原型这个概念， `cminus-f` 不区分函数的声明和定义。

4. $`\text{var-declaration}\ \rightarrow \text{type-specifier}\ \textbf{ID}\ \textbf{;}\ |\ \text{type-specifier}\ \textbf{ID}\ \textbf{[}\ \textbf{INTEGER}\ \textbf{]}\ \textbf{;}`$
5. $`\text{type-specifier} \rightarrow \textbf{int}\ |\ \textbf{float}\ |\ \textbf{void}`$

  `cminus-f` 的基础类型只有整型(int)、浮点型(float)和 `void`。而在变量声明中，只有整型和浮点型可以使用，`void` 仅用于函数声明。

  一个`变量声明`定义一个整型或者浮点型的变量，或者一个整型或浮点型的数组变量（这里整型指的是32位有符号整型，浮点数是指32位浮点数）。

  数组变量在声明时，$`\textbf{INTEGER}`$ 应当大于0。

  一次只能声明一个变量。

6. $`\text{fun-declaration} \rightarrow \text{type-specifier}\ \textbf{ID}\ \textbf{(}\ \text{params}\ \textbf{)}\ \text{compound-stmt}`$
7. $`\text{params} \rightarrow \text{param-list}\ |\ \textbf{void}`$
8. $`\text{param-list} \rightarrow \text{param-list}\ ,\ \text{param}\ |\ \text{param}`$
9. $`\text{param} \rightarrow \text{type-specifier}\ \textbf{ID}\ |\ \text{type-specifier}\ \textbf{ID}\ \textbf{[]}`$

  `函数声明`包含了返回类型，标识符，由逗号分隔的`形参`列表，还有一个`复合语句`。

  当函数的返回类型是 `void` 时，函数不返回任何值。

  函数的参数可以是 `void` ，也可以是一个列表。当函数的`形参`是`void`时，调用该函数时不用传入任何参数。

  `形参`中跟着中括号代表数组参数，它们可以有不同长度。

  整型参数通过值来传入函数（pass by value），而数组参数通过引用来传入函数（pass by reference，即指针）。

  函数的`形参`拥有和`函数声明`的`复合语句`相同的作用域，并且每次函数调用都会产生一组独立内存的参数。（和C语言一致）

  函数可以递归调用。

10. $`\text{compound-stmt} \rightarrow \textbf{\{}\ \text{local-declarations}\ \text{statement-list} \textbf{\}}`$

  一个`复合语句`由一对大括号和其中的`局部声明`与`语句列表`组成

  `复合语句`的执行时，对包含着的语句按照`语句列表`中的顺序执行

  `局部声明`拥有和`复合语句`中的`语句列表`一样的作用域，且其优先级高于任何同名的全局声明（常见的静态作用域）

11. $`\text{local-declarations} \rightarrow \text{local-declarations var-declaration}\ |\ \text{empty}`$
12. $`\text{statement-list} \rightarrow \text{statement-list}\ \text{statement}\ |\ \text{empty}`$

  `局部声明`和`语句列表`都可以为空（empty表示空字符串，即$`\varepsilon`$）

13. $`\begin{aligned}\text{statement} \rightarrow\ &\text{expression-stmt}\\ &|\ \text{compound-stmt}\\ &|\ \text{selection-stmt}\\ &|\ \text{iteration-stmt}\\ &|\ \text{return-stmt}\end{aligned}`$
14. $`\text{expression-stmt} \rightarrow \text{expression}\ \textbf{;}\ |\ \textbf{;}`$

 `表达式语句`由一个可选的`表达式`（即可以没有`表达式`）和一个分号组成

  我们通常使用`表达式语句`中的`表达式`计算时产生的副作用，所以这种`语句`用于赋值和函数调用

15. $`\begin{aligned}\text{selection-stmt} \rightarrow\ &\textbf{if}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}\\ &|\ \textbf{if}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}\ \textbf{else}\ \text{statement}\end{aligned}`$

  `if`语句中的`表达式`将被求值，若结果的值等于0，则第二个`语句`执行（如果存在的话），否则第一个`语句`会执行。

  为了避免歧义，$`\textbf{else}`$将会匹配最近的$`\textbf{if}`$

16. $`\text{iteration-stmt} \rightarrow \textbf{while}\ \textbf{(}\ \text{expression}\ \textbf{)}\ \text{statement}`$

  `while`语句是 `cminus-f` 中唯一的`迭代语句`。它执行时，会不断对`表达式`进行求值，并且在对`表达式`的求值结果等于 0 前，循环执行执下面的`语句`

17. $`\text{return-stmt} \rightarrow \textbf{return}\ \textbf{;}\ |\ \textbf{return}\ \text{expression}\ \textbf{;}`$

  `return`语句可以返回值，也可以不返回值。

  未声明为$`\textbf{void}`$类型的函数必须返回和函数返回类型相同的值

  `return`会将程序的控制转移给当前函数的调用者，而$`\textbf{main}`$函数的`return`会使得程序终止

18. $`\text{expression} \rightarrow \text{var}\ \textbf{=}\ \text{expression}\ |\ \text{simple-expression}`$
19. $`\text{var} \rightarrow \textbf{ID}\ |\ \textbf{ID}\ \textbf{[}\ \text{expression} \textbf{]}`$

  一个`表达式`可以是一个变量引用（即`var`）接着一个赋值符号（=）以及一个表达式，也可以是一个`简单表达式`。

  `var` 可以是一个整型变量、浮点变量，或者一个取了下标的数组变量。

  数组的下标值是整型，它的值是表达式计算结果或结果进行类型转换后的整型值

  一个负的下标会导致程序终止，需要调用框架中的内置函数`neg_idx_except` （该内部函数会主动退出程序，只需要调用该函数即可），但是对于上界并不做检查。

  赋值语义为：先找到 `var` 代表的变量地址（如果是数组，需要先对下标表达式求值），然后对右侧的表达式进行求值，求值结果将在转换成变量类型后存储在先前找到的地址中。同时，存储在 `var` 中的值将作为赋值表达式的求值结果。

  在 `C` 中，赋值对象（即 `var` ）必须是左值，而左值可以通过多种方式获得。`cminus-f`中，唯一的左值就是通过 `var` 的语法得到的，因此 `cminus-f` 通过语法限制了 `var` 为左值，而不是像 `C` 中一样通过类型检查，这也是为什么 `cminus-f` 中不允许进行指针算数。

20. $`\text{simple-expression} \rightarrow \text{additive-expression}\ \text{relop}\ \text{additive-expression}\ |\ \text{additive-expression}`$
21. $`\text{relop}\ \rightarrow \textbf{<=}\ |\ \textbf{<}\ |\ \textbf{>}\ |\ \textbf{>=}\ |\ \textbf{==}\ |\ \textbf{!=}`$
22. $`\text{additive-expression} \rightarrow \text{additive-expression}\ \text{addop}\ \text{term}\ |\ \text{term}`$
23. $`\text{addop} \rightarrow \textbf{+}\ |\ \textbf{-}`$
24. $`\text{term} \rightarrow \text{term}\ \text{mulop}\ \text{factor}\ |\ \text{factor}`$
25. $`\text{mulop} \rightarrow \textbf{*}\ |\ \textbf{/}`$

  一个`简单表达式`是一个`加法表达式`或者两个`加法表达式`的关系运算。当它是`加法表达式`时，它的值就是`加法表达式`的值。而当它是关系运算时，如果关系运算结果为真则值为整型值 1，反之则值为整型值 0。

  `加法表达式`表现出了四则运算的结合性质与优先级顺序，四则运算的含义和`C`中的整型运算一致。

  浮点数和整型一起运算时，整型值需要进行类型提升，转换成浮点数类型，且运算结果也是浮点数类型

26. $`\text{factor} \rightarrow \textbf{(}\ \text{expression}\ \textbf{)}\ |\ \text{var}\ |\ \text{call}\ |\ \text{integer}\ |\ \text{float}`$

  `因数`可以是一个括号包围的`表达式`（此时它的值是`表达式`的值），或者是一个`变量`（此时它的值是`变量`的值），或者是一个`函数调用`（此时它的值是`函数调用`的返回值），或者是一个`数字字面量`（此时它的值为该字面量的值）。当`因数`是数组变量时，除非此时它被用作一个`函数调用`中的数组参数，否则它必须要带有下标。

27. $`\text{integer} \rightarrow \textbf{INTEGER}`$
28. $`\text{float} \rightarrow \textbf{FLOATPOINT}`$
29. $`\text{call} \rightarrow \textbf{ID}\ \textbf{(}\ \text{args} \textbf{)}`$
30. $`\text{args} \rightarrow \text{arg-list}\ |\ \text{empty}`$
31. $`\text{arg-list} \rightarrow \text{arg-list}\ \textbf{,}\ \text{expression}\ |\ \text{expression}`$

`函数调用`由一个函数的`标识符`与一组括号包围的`实参`组成。`实参`可以为空，也可以是由逗号分隔的的`表达式`组成的列表，这些表达式代表着函数调用时，传给`形参`的值。`函数调用时`的`实参`数量和类型必须与`函数声明`中的`形参`一致，必要时需要进行类型转换。

`cminus-f`中包含四个预定义的函数 `input` 、 `output`、 `outputFloat` 和 `neg_idx_except`，它们的声明为:

```c
int input(void) {...}
void output(int x) {...}
void outputFloat(float x) {...}
void neg_idx_except(void) {...}
```

* `input` 函数没有形参，且返回一个从标准输入中读到的整型值。
* `output` 函数接受一个整型参数，然后将它的值打印到标准输出，并输出换行符。
* `outputFloat` 函数接受一个浮点参数，然后将它的值打印到标准输出，并输出换行符。
* `neg_idx_except` 函数没有形参，执行后报错并退出


除此之外，其它规则和 C 中类似，比如同一个作用域下不允许定义重名变量或函数（本次实验中不做要求）

## 提醒与补充
1. 本次实验存在五种情况下的类型转换
    * 赋值时
    * 返回值类型和函数签名中的返回类型不一致时
    * 函数调用时实参和函数签名中的形参类型不一致时
    * 二元运算的两个参数类型不一致时
    * 下标计算时
2. 如果对上述的语义有疑问可以通过发issue的方式进行交流（当然，我们推荐组内先进行讨论）。
