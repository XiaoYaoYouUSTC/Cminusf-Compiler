# Lab3 实验文档
本次实验由金泽文、陈清源、李嘉豪共同设计。
- [Lab3 实验文档](#lab3-实验文档)
  - [0. 前言](#0-前言)
    - [主要工作](#主要工作)
  - [1. LLVM IR部分](#1-llvm-ir部分)
    - [1.1 LLVM IR介绍](#11-llvm-ir介绍)
    - [1.2 gcd例子: 利用clang生成的.ll](#12-gcd例子-利用clang生成的ll)
    - [1.3 你的提交1: 手动编写.ll](#13-你的提交1-手动编写ll)
  - [2. LightIR部分](#2-lightir部分)
    - [2.1 LightIR - LLVM IR的C++接口](#21-lightir---llvm-ir的c接口)
    - [2.2 gcd例子: 利用LightIR + cpp 生成.ll](#22-gcd例子-利用lightir--cpp-生成ll)
    - [2.3 你的提交2: 利用LightIR + cpp编写生成.ll的程序](#23-你的提交2-利用lightir--cpp编写生成ll的程序)
  - [3. Lab4的准备](#3-lab4的准备)
    - [3.1 了解Visitor Pattern](#31-了解visitor-pattern)
    - [3.2 你的提交3: 组队信息](#32-你的提交3-组队信息)
  - [4. 实验要求](#4-实验要求)
    - [4.1 目录结构](#41-目录结构)
    - [4.2 编译、运行和验证](#42-编译运行和验证)
    - [4.3 提交要求和评分标准](#43-提交要求和评分标准)
## 0. 前言

本次实验作为Lab4的前驱实验，独立于Lab1、Lab2，依旧是个人实验。Lab4开始为组队实验。    
本次实验的目的是让大家熟悉Lab4所需要的相关知识: LLVM IR、 LightIR（LLVM IR的轻量级C++接口）和 Visitor Pattern（访问者模式）。  
在开始实验之前，如果你使用的不是助教提供的虚拟机镜像，请根据之前的[Issue - 环境准备](http://222.195.68.197/staff/2020-fall-notice_board/-/issues/13)确保LLVM的版本为10.0.1，且PATH环境变量配置正确。可以通过`lli --version`命令是否可以输出10.0.1的版本信息来验证。

### 主要工作

1. 第一部分: 了解LLVM IR。通过clang生成的.ll，了解LLVM IR与c代码的对应关系。完成1.3
2. 第二部分: 了解LightIR。通过助教提供的c++例子，了解LightIR的c++接口及实现。完成2.3
3. 第三部分: 理解Visitor Pattern。提交组队信息，完成3.2。
4. 实验报告：在[report.md](./Reports/lab3/report.md)中**回答3个问题**。

## 1. LLVM IR部分
### 1.1 LLVM IR介绍
根据[维基百科](https://zh.wikipedia.org/zh-cn/LLVM)的介绍，LLVM是一个自由软件项目，它是一种编译器基础设施，以C++写成，包含一系列模块化的编译器组件和工具链，用来开发编译器前端和后端。IR的全称是Intermediate Representation，即中间表示。LLVM IR是一种类似于汇编的底层语言。  
LLVM IR的具体指令可以参考[Reference Manual](http://llvm.org/docs/LangRef.html)。但是你会发现其内容庞杂。虽然助教认为，高效地查阅官方文档及手册是非常必要的一项技能，但是由于其手册过于复杂，因此助教筛选了后续实验中将要用到的子集，总结为了[精简的IR Reference手册](../common/LightIR.md)。  
作为一开始的参考，你可以先阅读其中`IR Features`和`IR Format`两节，后续有需要再反复参考。实验的最后，你需要在[report.md](./Reports/lab3/report.md)中**回答问题3**。  

### 1.2 gcd例子: 利用clang生成的.ll
阅读[tests/lab3/ta_gcd/gcd_array.c](../../tests/lab3/ta_gcd/gcd_array.c)。  
根据`clang -S -emit-llvm gcd_array.c`指令，你可以得到对应的`gcd_array.ll`文件.你需要结合[gcd_array.c](../../tests/lab3/ta_gcd/gcd_array.c)阅读`gcd_array.ll`，理解其中每条LLVM IR指令与c代码的对应情况。  
通过`lli gcd_array.ll; echo $?`指令，你可以测试`gcd_array.ll`执行结果的正确性。其中，

- `lli`会运行`*.ll`文件
- `$?`的内容是上一条命令所返回的结果，而`echo $?`可以将其输出到终端中   

后续你会经常用到这两条指令。  

### 1.3 你的提交1: 手动编写.ll
助教提供了四个简单的c程序，分别是`tests/lab3/c_cases/`目录下的[assign.c](../../tests/lab3/c_cases/assign.c)、[fun.c](../../tests/lab3/c_cases/fun.c)、[if.c](../../tests/lab3/c_cases/if.c)和[while.c](../../tests/lab3/c_cases/while.c).你需要在`tests/lab3/stu_ll/`目录中，手工完成自己的[assign_hand.ll](../../tests/lab3/stu_ll/assign_hand.ll)、[fun_hand.ll](../../tests/lab3/stu_ll/fun_hand.ll)、[if_handf.ll](../../tests/lab3/stu_ll/if_hand.ll)和[while_hand.ll](../../tests/lab3/stu_ll/while_hand.ll)，以实现与上述四个C程序相同的逻辑功能.你需要添加必要的注释.`.ll`文件的注释是以";"开头的。  
必要的情况下，你可以参考`clang -S -emit-llvm`的输出，但是你提交的结果必须避免同此输出一字不差。  
助教会用`lli`检查你结果的正确性，并用肉眼检查你的注释。

## 2. LightIR部分
### 2.1 LightIR - LLVM IR的C++接口
由于LLVM IR官方的C++接口的文档同样过于冗长，助教提供了`LightIR`这一C++接口库。你需要阅读[LightIR核心类的介绍](../common/LightIR.md)。  
lab4部分会要求大家通过`LightIR`根据`AST`构建生成LLVM IR。所以你需要仔细阅读文档了解其接口的设计。

### 2.2 gcd例子: 利用LightIR + cpp 生成.ll
为了让大家更直观地感受并学会`LightIR`接口的使用，助教提供了[tests/lab3/ta_gcd/gcd_array_generator.cpp](../../tests/lab3/ta_gcd/gcd_array_generator.cpp)。该cpp程序会生成与gcd_array.c逻辑相同的LLVM IR文件。助教提供了非常详尽的注释，一定要好好利用！    
该程序的编译与运行请参考4.2节。

### 2.3 你的提交2: 利用LightIR + cpp编写生成.ll的程序
你需要在`tests/lab3/stu_cpp/`目录中，编写[assign_generator.cpp](../../tests/lab3/stu_cpp/assign_generator.cpp)、[fun_generator.cpp](../../tests/lab3/stu_cpp/fun_generator.cpp)、[if_generator.cpp](../../tests/lab3/stu_cpp/if_generator.cpp)和[while_generator.cpp](../../tests/lab3/stu_cpp/while_generator.cpp)，以生成与1.3节的四个C程序相同逻辑功能的`.ll`文件。你需要添加必要的注释。你需要在[report.md](./Reports/lab3/report.md)中**回答问题1**。

## 3. Lab4的准备
### 3.1 了解Visitor Pattern
Visitor Pattern(访问者模式)是一种在LLVM项目源码中被广泛使用的设计模式。在遍历某个数据结构（比如树）时，如果我们需要对每个节点做一些额外的特定操作，Visitor Pattern就是个不错的思路。  
Visitor Pattern是为了解决**稳定的数据结构**和**易变的操作耦合问题**而产生的一种设计模式。解决方法就是在被访问的类里面加一个对外提供接待访问者的接口，其关键在于在数据基础类里面有一个方法接受访问者，将自身引用传入访问者。这里举一个应用实例来帮助理解访问者模式: 您在朋友家做客，您是访问者；朋友接受您的访问，您通过朋友的描述，然后对朋友的描述做出一个判断，这就是访问者模式。  
有关 Visitor Pattern 的含义、模式和特点，有梯子的同学可参考[维基百科](https://en.wikipedia.org/wiki/Visitor_pattern#C++_example)。  
下面的例子可以清晰地展示Visitor Pattern的运作方式。这是助教编写的计算表达式 `4 * 2 - 2 / 4 + 5` 结果的C++程序。  
其中较为重要的一点原则在于，C++中对函数重载特性的支持。在代码`treeVisitor.visit(node)`中，根据`node`对象具体类型的不同，编译器会在`visit(AddSubNode& node)`、`visit(NumberNode& node)`、`visit(MulDivNode& node)`三者中，选择对应的实现进行调用。你需要理解下面这个例子中tree是如何被遍历的。请在[report.md](./Reports/lab3/report.md)中**回答问题2**。

<details>
  <summary>例子:简单的表达式计算 - visitor.cpp</summary>

``` c++
#include <iostream>
#include <vector>

class TreeVisitor;  // Forward declare TreeVisitor

class Node {  // Parent class for the elements (AddSubNode, NumberNode and
              // MulDivNode)
 public:
  // This function accepts an object of any class derived from
  // TreeVisitor and must be implemented in all derived classes
  virtual int accept(TreeVisitor& treeVisitor) = 0;
};

// Forward declare specific elements (nodes) to be dispatched
class AddSubNode;
class NumberNode;
class MulDivNode;

class TreeVisitor {  // Declares the interface for the treeVisitor
 public:
  // Declare overloads for each kind of a node to dispatch
  virtual int visit(AddSubNode& node) = 0;
  virtual int visit(NumberNode& node) = 0;
  virtual int visit(MulDivNode& node) = 0;
};

class AddSubNode : public Node {  // Specific element class #1
 public:
  // Resolved at runtime, it calls the treeVisitor's overloaded function,
  // corresponding to AddSubNode.
  int accept(TreeVisitor& treeVisitor) override {
    return treeVisitor.visit(*this);
  }
  Node& leftNode;
  Node& rightNode;
  std::string op;
  AddSubNode(Node& left, Node& right, std::string op): leftNode(left), rightNode(right), op(op){}
};

class NumberNode : public Node {  // Specific element class #2
 public:
  // Resolved at runtime, it calls the treeVisitor's overloaded function,
  // corresponding to NumberNode.
  int accept(TreeVisitor& treeVisitor) override {
    return treeVisitor.visit(*this);
  }
  int number;
  NumberNode(int number){
    this->number = number;
  }
};

class MulDivNode : public Node {  // Specific element class #3
 public:
  // Resolved at runtime, it calls the treeVisitor's overloaded function,
  // corresponding to MulDivNode.
  int accept(TreeVisitor& treeVisitor) override {
    return treeVisitor.visit(*this);
  }
  Node& leftNode;
  Node& rightNode;
  std::string op;
  MulDivNode(Node& left, Node& right, std::string op): leftNode(left), rightNode(right), op(op){}
};

class TreeVisitorCalculator : public TreeVisitor {  // Implements triggering of all
                                                // kind of elements (nodes)
 public:
  int visit(AddSubNode& node) override {
    auto right = node.rightNode.accept(*this);
    auto left = node.leftNode.accept(*this);
    if (node.op == "add") {
      return left + right;
    } 
    else {
      return left - right;
    }
  }

  int visit(NumberNode& node) override {
    return node.number;
  }

  int visit(MulDivNode& node) override {
    auto left = node.leftNode.accept(*this);
    auto right = node.rightNode.accept(*this);
    if (node.op == "mul") {
      return left * right;
    } 
    else {
      return left / right;
    }
  }
};

int main() {
  // construct the expression nodes and the tree
  // the expression: 4 * 2 - 2 / 4 + 5
  auto numberA = NumberNode(4);
  auto numberB = NumberNode(2);
  auto exprC = MulDivNode(numberA, numberB, "mul");
  auto exprD = MulDivNode(numberB, numberA, "div");
  auto exprE = AddSubNode(exprC, exprD, "sub");
  auto numberF = NumberNode(5);
  auto exprRoot = AddSubNode(exprE, numberF, "add");

  TreeVisitorCalculator treeVisitor;
  // traverse the tree and calculate
  int result = treeVisitor.visit(exprRoot);
  std::cout << "4 * 2 - 2 / 4 + 5 evaluates: " << result << std::endl;
  return 0;
}
```
</details>

该文件的执行结果如下:
``` shell
$ g++ visitor.cpp -std=c++14; ./a.out
4 * 2 - 2 / 4 + 5 evaluates: 13
```

### 3.2 你的提交3: 组队信息
阅读[Issue - 组队规则](http://222.195.68.197/staff/2020-fall-notice_board/-/issues/89)，找到你的队友。如果lab3 soft ddl之后你还没有找到队友，那么助教将会为你随机分配队友。
## 4. 实验要求

### 4.1 目录结构
除了下面指明你所要修改或提交的文件，其他文件请勿修改。
``` log
.
├── CMakeLists.txt
├── Documentations
│   ├── ...
|   ├── common                          <- LightIR 相关文档
│   └── lab3
│       └── README.md                   <- lab3实验文档说明（你在这里）
├── include                             <- 实验所需的头文件
│   ├── ...
│   ├── lightir
├── README.md
├── Reports
│   ├── ...
│   └── lab3
│       ├── report.md                   <- lab3所需提交的实验报告，含3个问题（你要交）
│       └── team.txt                    <- lab3所需提交的组队信息（你要交）
├── src
│   ├── ...
│   └── lightir
└── tests
    ├── CMakeLists.txt
    ├── ...
    └── lab3                            <- lab3文件夹
        ├── c_cases                     <- 4个c程序
        │   ├── assign.c
        │   ├── fun.c
        │   ├── if.c
        │   └── while.c
        ├── CMakeLists.txt              <- 你在2.3节需要去掉注释（我们不收，你要改）
        ├── stu_cpp                     <- lab3所需提交的cpp目录（你要交）
        │   ├── assign_generator.cpp
        │   ├── fun_generator.cpp
        │   ├── if_generator.cpp
        │   └── while_generator.cpp
        ├── stu_ll                      <- lab3所需提交的.ll目录（你要交）
        │   ├── assign_hand.ll
        │   ├── fun_hand.ll
        │   ├── if_hand.ll
        │   └── while_hand.ll
        └── ta_gcd
            ├── gcd_array.c
            └── gcd_array_generator.cpp <- 助教提供的生成gcd_array.ll的cpp
```

### 4.2 编译、运行和验证

* 编译与运行
  在 `${WORKSPACE}/build/` 下执行:
  ``` shell
  # 如果存在 CMakeCache.txt 要先删除
  # rm CMakeCache.txt
  cmake ..
  make
  make install
  ```
  你可以得到对应`gcd_array_generator.cpp`的可执行文件。  
  在完成2.3时，在`${WORKSPACE}/tests/lab3/CMakeLists.txt`中去掉对应的注释，再在`${WORKSPACE}/build/`下执行`cmake ..`与`make`指令，即可得到对应的可执行文件。
* 验证  
  本次试验测试案例只有`${WORKSPACE}/tests/lab3/c_cases`中的4个样例。请大家自行验证。  
  助教会执行你们的代码，并使用 `diff` 命令进行验证。
  
### 4.3 提交要求和评分标准
* 提交要求  
  本实验的提交要求分为两部分: 实验部分的文件和报告，git提交的规范性。
  * 实验部分:
    * 需要完成 `./tests/lab3/stu_ll`目录下的4个文件
    * 需要完成 `./tests/lab3/stu_cpp`目录下的4个文件
    * 需要在 `./Report/lab3/` 目录下撰写实验报告，并提交组队信息
      * 实验报告内容包括:
        * 实验要求、3个问题、实验难点、实验反馈(具体参考[report.md](./Reports/lab3/report.md))
        * 本次实验报告**参与**评分标准.
    * 本次实验收取 `./tests/lab3/stu_ll` 目录、`./tests/lab3/stu_cpp` 目录和 `./Report/lab3` 目录
  * git提交规范: 
    * 不破坏目录结构(`report.md`如果需要放图片，请放在`./Reports/lab3/figs/`下)
    * 不上传临时文件(凡是自动生成的文件和临时文件请不要上传)
    * git log言之有物(不强制，请不要git commit -m 'commit 1'，git commit -m 'sdfsdf'，每次commit请提交有用的comment信息)
* 评分标准: 本次实验的测试样例较为简单，所以为了拿高分请注意report.md
  * 1.3节`.ll`运行结果正确(一个5分，共20分)
  * 2.3节`.cpp`运行结果正确(一个10分，共40分)
  * `report.md`(40分)
  * 禁止执行恶意代码，违者本次实验0分处理
* 迟交规定
  * `Soft Deadline` : 2020/11/16 23:59:59 (北京标准时间，UTC+8)
  * `Hard Deadline` : 2020/11/23 23:59:59 (北京标准时间，UTC+8)
  * 迟交需要邮件通知TA: 
    * 邮箱: king.zevin@qq.com
    * 邮件主题: lab3迟交-学号
    * 内容: 包括迟交原因、最后版本commitID、迟交时间等
  * 迟交分数
    * x为迟交天数(对于`Soft Deadline`而言)，grade满分10 
      ``` bash
      final_grade = grade, x = 0
      final_grade = grade * (0.9)^x, 0 < x <= 7
      final_grade = 0, x > 7 # 这一条严格执行,请对自己负责
      ```
* 关于抄袭和雷同
  经过助教和老师判定属于作业抄袭或雷同情况，所有参与方一律零分，不接受任何解释和反驳。
如有任何问题，欢迎提issue进行批判指正。
