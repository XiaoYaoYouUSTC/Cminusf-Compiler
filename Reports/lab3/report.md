# lab3 实验报告
学号 PB18000051 姓名 陈俊驰

## 问题1: cpp与.ll的对应
请描述你的cpp代码片段和.ll的每个BasicBlock的对应关系。描述中请附上两者代码。

**答：** 以下为.cpp代码片段和.ll的BasicBlock的对应关系，为了不靠代码水太长且方便描述，可能相较于源代码会适当减少一点注释

**assign_generator.cpp和assign.ll**

在assign.ll文件中仅含有一个BasicBlock,在main函数中，名为entry，其代码为：

``` 
entry:
  %0 = alloca i32
  %1 = alloca [10 x i32]
  store i32 0, i32* %0
  %2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 0
  store i32 10, i32* %2
  %3 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 0
  %4 = load i32, i32* %3
  %5 = mul i32 %4, 2
  %6 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 1
  store i32 %5, i32* %6
  %7 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 1
  %8 = load i32, i32* %7
  ret i32 %8
  ```

对应的cpp代码如下：
  ```
  //定义BasicBlock，名为entry，并将插入点移到entry上
  auto bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //对应ll中对于ret和a[10]的申请
  auto retAlloca = builder->create_alloca(Int32Type);
  auto *arrayType = ArrayType::get(Int32Type,10);
  auto a = builder->create_alloca(arrayType);
  //ret 0
  builder->create_store(CONST_INT(0),retAlloca);
  //令a[0]=10
  auto a0GEP = builder->create_gep(a,{CONST_INT(0),CONST_INT(0)});
  builder->create_store(CONST_INT(10),a0GEP);
  //计算a[0]*2
  a0GEP = builder->create_gep(a,{CONST_INT(0),CONST_INT(0)});
  auto a0Load = builder->create_load(a0GEP);
  auto mul=builder->create_imul(a0Load,CONST_INT(2));
  //令a[1]=a[0]*2
  auto a1GEP = builder->create_gep(a,{CONST_INT(0),CONST_INT(1)});
  builder->create_store(mul,a1GEP);
  //return a[1]
  a1GEP = builder->create_gep(a,{CONST_INT(0),CONST_INT(1)});
  auto a1Load = builder->create_load(a1GEP);
  builder->create_ret(a1Load);
  ```

​	在assign_generator.cpp文件中，通过上述代码生成assign.ll文件中的基本块entry。具体语句的对应见cpp注释。



**fun_generator.cpp和fun.ll**

在fun.ll中含有两个BasicBlock，一个位于函数callee中名为entry，另一个位于main中名为entry

callee中的entry BasicBlock代码为：

```
entry:
  %1 = alloca i32
  %2 = alloca i32
  store i32 0, i32* %1
  store i32 %0, i32* %2
  %3 = load i32, i32* %2
  %4 = mul i32 2, %3
  ret i32 %4
  ```

对应的cpp代码如下：

```
 //定义函数callee中的Basicblock
  auto bb = BasicBlock::create(module,"entry",calleeFun);
  builder->set_insert_point(bb);
  //为ret和a分配内存
  auto retAlloca = builder->create_alloca(Int32Type);
  auto aAlloca = builder->create_alloca(Int32Type);
  //ret 0
  builder->create_store(CONST_INT(0),retAlloca);
  //获得函数callee的形参
  std::vector<Value *> args;//获取callee函数的形参
  for (auto arg = calleeFun->arg_begin(); arg != calleeFun->arg_end(); arg++) {
    args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
  }
  //获得a的值
  builder->create_store(args[0],aAlloca);
  //计算2*a
  auto aLoad=builder->create_load(aAlloca);
  auto mul=builder->create_imul(CONST_INT(2),aLoad);
  //return 2*a
  builder->create_ret(mul);
  ```

​	在fun_generator.cpp文件中，通过上述代码生成fun.ll文件中的callee基本块entry。具体语句的对应见cpp注释。

main函数中的entry BasicBlock代码如下：

```
entry:
  %0 = alloca i32
  store i32 0, i32* %0
  %1 = call i32 @callee(i32 110)
  ret i32 %1
  ```

对应的cpp代码如下：

```
//定义basicblock
  bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //ret 0
  retAlloca=builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0),retAlloca);
  //调用callee函数并获得返回值
  auto call = builder->create_call(calleeFun,{CONST_INT(110)});
  //返回调用的结果
  builder->create_ret(call);
  ```

在fun_generator.cpp文件中，通过上述代码生成fun.ll文件中的main函数基本块entry。具体语句的对应见cpp注释。



**if_generator.cpp和if.ll**

在if.ll文件中共有四个BasicBlock，分别为对应if判断语句及以前的entry BasicBlock、对应if判断为真的分支的trueBB BasicBlock、对应if判断为假的分支falseBB BasicBlock以及对应return功能的返回分支4 BasicBlock。

其中，entry BasicBlock的代码如下:

```
entry:
  %0 = alloca i32  
  %1 = alloca float  
  store i32 0, i32* %0  
  store float 0x40163851e0000000, float* %1  
  %2 = load float, float* %1  
  %3 = fcmp ugt float %2,0x3ff0000000000000  
  br i1 %3, label %trueBB, label %falseBB
  ```

对应的cpp代码如下:

```
//定义basicblock
  auto bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //定义ret和a 
  auto retAlloca=builder->create_alloca(Int32Type);
  auto aAlloca=builder->create_alloca(FloatType);
  //ret 0 和 a=5.555
  builder->create_store(CONST_INT(0),retAlloca);
  builder->create_store(CONST_FP(5.555),aAlloca);
  //比较a和1
  auto aLoad =builder->create_load(aAlloca);
  auto fcmp =builder->create_fcmp_gt(aLoad,CONST_FP(1.0));
  //定义BasicBlock trueBB ,falseBB和retBB
  auto trueBB = BasicBlock::create(module, "trueBB", mainFun);    // true分支
  auto falseBB = BasicBlock::create(module, "falseBB", mainFun);  // false分支
  auto retBB = BasicBlock::create(module, "", mainFun); // return分支
  //根据if语句判定结果决定前往trueBB还是falseBB
  builder->create_cond_br(fcmp,trueBB,falseBB);
  ```

在if_generator.cpp文件中，通过上述代码生成if.ll文件中的基本块entry。具体语句对应见注释。

trueBB BasicBlock的代码如下:

```
trueBB:
  store i32 233, i32* %0
  br label %4
  ```

对应的cpp代码如下:

``` 
//trueBB,ret 233
  builder->set_insert_point(trueBB);
  //将返回变量赋值233，并进入返回分支，即4BasicBlock
  builder->create_store(CONST_INT(233),retAlloca);
  builder->create_br(retBB);
  ```

在if_generator.cpp文件中，通过上述代码生成if.ll文件中的基本块trueBB。具体语句对应见注释。

falseBB BasicBlock的代码如下:

```
falseBB:
  store i32 0, i32* %0
  br label %4
  ```

对应的cpp代码如下:

```
//falseBB
  builder->set_insert_point(falseBB);
  //将返回变量赋值0，并进入返回分支，即4BasicBlock
  builder->create_store(CONST_INT(0),retAlloca);
  builder->create_br(retBB);
  ```

在if_generator.cpp文件中，通过上述代码生成if.ll文件中的基本块falseBB。具体语句对应见注释。

4 BasicBlock的代码如下:

```
4:
  %5 = load i32, i32* %0
  ret i32 %5
  ```

对应的cpp代码如下:

``` 
 //retBB
  builder->set_insert_point(retBB);
  //将返回变量所存储的返回值返回
  auto retLoad=builder->create_load(retAlloca);
  builder->create_ret(retLoad);
  ```

在if_generator.cpp文件中，通过上述代码生成if.ll文件中的基本块4。具体语句对应见注释。



**while_generator.cpp和while.ll**

在while.ll文件中总共有3个BasicBlock，分别为对应第一次循环条件判断及以前语句的entry BasicBlock、对应循环动作及判断是否要离开循环的loopBB BasicBlock 、对应循环结束后语句的outBB BasicBlock。

其中，entry BasicBlock的代码如下：

```
entry:
  %0 = alloca i32
  %1 = alloca i32
  %2 = alloca i32
  store i32 0, i32* %0
  store i32 10, i32* %1
  store i32 0, i32* %2
  %3 = load i32, i32* %2
  %4 = icmp slt i32 %3, 10
  br i1 %4, label %loopBB, label %outBB
  ```

对应cpp代码：

```
//定义entry basicblock
  auto bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //定义ret,a,i
  auto retAlloca = builder->create_alloca(Int32Type);
  auto aAlloca = builder->create_alloca(Int32Type);
  auto iAlloca = builder->create_alloca(Int32Type);
  //赋值ret 0,a 10,i 0
  builder->create_store(CONST_INT(0),retAlloca);
  builder->create_store(CONST_INT(10),aAlloca);
  builder->create_store(CONST_INT(0),iAlloca);
  //比较i与10
  auto iLoad=builder->create_load(iAlloca);
  auto icmp=builder->create_icmp_lt(iLoad,CONST_INT(10));
   //定义loopBB ,outBB
  auto loopBB = BasicBlock::create(module, "loopBB", mainFun);    // loop分支
  auto outBB = BasicBlock::create(module, "outBB", mainFun);  // out分支
  //决定是否进入循环基本块loopBB
  builder->create_cond_br(icmp,loopBB,outBB);
  ```

在will_generator.cpp文件中，通过上述代码生成will.ll文件中的基本块entry。具体语句对应见注释。

loopBB BasicBlock的代码如下：

```
loopBB:
  %5 = load i32, i32* %2
  %6 = add i32 %5, 1
  store i32 %6, i32* %2
  %7 = load i32, i32* %1
  %8 = load i32, i32* %2
  %9 = add i32 %7, %8
  store i32 %9, i32* %1
  %10 = load i32, i32* %2
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %loopBB, label %outBB
  ```

对应cpp代码：

```
  //loop branch
  builder->set_insert_point(loopBB);
  //令i=i+1
  iLoad=builder->create_load(iAlloca);
  auto add=builder->create_iadd(iLoad,CONST_INT(1));
  builder->create_store(add,iAlloca);
  //令a=a+i
  auto aLoad=builder->create_load(aAlloca);
  iLoad=builder->create_load(iAlloca);
  add=builder->create_iadd(aLoad,iLoad);
  builder->create_store(add,aAlloca);
  //比较i和10，决定是否离开循环进入outBB基本块
  iLoad=builder->create_load(iAlloca);
  icmp=builder->create_icmp_lt(iLoad,CONST_INT(10));
  builder->create_cond_br(icmp,loopBB,outBB);
  ```

在will_generator.cpp文件中，通过上述代码生成will.ll文件中的基本块loopBB。具体语句对应见注释。

outBB BasicBlock的代码如下：

```
outBB:
  %12 = load i32, i32* %1
  ret i32 %12
  ```

对应cpp代码：

```
  //outBB
  builder->set_insert_point(outBB);
  //返回结果a
  aLoad=builder->create_load(aAlloca);
  builder->create_ret(aLoad);
  ```

在will_generator.cpp文件中，通过上述代码生成will.ll文件中的基本块outBB。具体语句对应见注释。

## 问题2: Visitor Pattern
请指出visitor.cpp中，`treeVisitor.visit(exprRoot)`执行时，以下几个Node的遍历序列:numberA、numberB、exprC、exprD、exprE、numberF、exprRoot。  
序列请按如下格式指明：  
exprRoot->numberF->exprE->numberA->exprD

**答：** 根据visit和accept可以很简单的判断

1.访问exprRoot时，由于为AddSubNode，先访问右子节点numberF，返回一个值，再访问左子节点exprE；

2.访问exprE时，由于为AddSubNode，先访问右子节点exprD，，再访问左子节点exprC；

3.访问exprD时，由于为MulDivNode，先访问左节点numberB，返回一个值，再访问右节点numberA，返回一个值；

4.访问exprC时，由于为MulDivNode，先访问左节点numberA，返回一个值，再访问右节点numberB，返回一个值；

由此，完成对于treeVisitor.visit(exprRoot)的执行，最终获得Node的遍历序列为：

exprRoot->numberF->exprE->exprD->numberB->numberA->exprC->numberA->numberB

## 问题3: getelementptr
请给出`IR.md`中提到的两种getelementptr用法的区别,并稍加解释:
  - `%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0` 
  - `%2 = getelementptr i32, i32* %1， i32 %0` 

**答：** 从结果的角度讲，两种getelementptr的用法的结果都是将一个i32*类型的，即一个i32内存的地址存入%2中。但在实际的用法中有着如下的区别：

**1.** 两种用法对于偏移值参数数量要求不同

​	对于`%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0`，将内存按照10\*i32的大小，即40字节，进行划分，而每个10\*i32大小的内存中又按照i32，即4字节，进行划分，分为10个i32大小。此时需要**两个**偏移类型及偏移值来获得所需地址。可以将整个内存看做一个二维数组，每一个元素代表一个i32大小内存，每一行为10个i32。其中第一个偏移值代表行号，以题目中i32 0为例，首先地址应当偏移0\*40=0个字节；第二个偏移值代表列号，以题目中i32 %0为例，地址应再偏移%0*4=0个字节。最终获得的地址偏移量为0\*40+0%\*40=0%\*40。（偏移是针对所给索引开始的指针%1的）

​	对于`%2 = getelementptr i32, i32* %1， i32 %0`将内存按照i32的大小，即4字节进行划分。此时需要**一个**偏移类型及偏移值来获取所需地址。可以将整个内存看做一个一维数组，每一个元素代表一个i32大小内存。其中偏移值代表元素的序号，以题目中i32 %0为例，地址应当偏移%0*4字节，即取第[%0]个i32所在的地址。

**2.** 两种用法对于索引开始的指针类型和计算基础类型要求不同

​	对于`%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0`，要求传入的%1必须为[10 x i32]* 类型；而对于`%2 = getelementptr i32, i32* %1， i32 %0`，要求传入的%1必须为i32*类型。

**3.** 两种用法的使用场景不同

​	对于`%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0`，由于要求传入的%1必须为[10 x i32]* 类型，则对于一个使用了上述方法的函数，该方法所针对的数组必须为一个在该函数中长度特定且已知的数组，例如在本函数中申请的a[10]或者全局数组b[5]等，而函数形参中的u[]所传递的数组由于长度未知，不能使用该方法。该方法兼容性较差，只能针对特定数组。

​	对于`%2 = getelementptr i32, i32* %1， i32 %0`，由于要求传入的%1必须为i32*类型，则对于一个使用了该方法的函数，该数组为一个在该函数中长度不确定的数组，例如：该数组为在其他函数中申请的数组，然后作为本函数的形参例如u[]（不确定长数组）的实参传入本函数。一个函数为了尽可能处理不同长度的数组，故使用该方法，因为兼容性较好，能够针对不同长度的数组

​	二者的使用区别有点类似于C语言中对于静态数组和动态数组的处理，一个是长度固定，一个是长度可变。

## 实验难点
描述在实验中遇到的问题、分析和解决方案

对于实验第1部分，难点主要在于读懂IR参考手册以及正确理解每一个三地址语句的功能用法。对于该问题，通过对比阅读gcd.ll和gcd.c文件，理解了如何正确使用并书写一个ll文件。由于对于C语言的使用有些生疏，导致对于地址、指针等知识点遗忘，也直接导致了对于getelementptr的使用出错，然后通过阅读网上的相关资料和官方手册对于其解释，最终理解到它获得的是一个地址而非所存储的值。在本实验中，对我产生困扰的最混乱的一个点在于返回变量、返回分支的申请，读了clang指令生成的ll文件，查了很多资料都没有得到何时需要申请，虽然通过测试，这二者并非是必须的。最后通过issue提问（赞美助教）和舍友的支援，并阅读了clang的相关资料最终了解了相关知识，这二者只是格式上的，而非功能上的必须部分。

对于实验第2部分，难点主要在于我常用的语言为Python和C，对于类并不了解，花费了部分时间初步了解了类。但是对于LightIR核心类的使用仅看参考资料仍然无法理解，最终再次使用对比阅读，阅读助教所提供的例子及ll和c文件，并使用了最开始学习编程的依葫芦画瓢模式，每次要写一段功能时，先去例子中找相关的部分，学习并借鉴，在写完一个generator后，就可以很轻松的自己写了。在实验过程中遇到了2个问题，第一个是alloca时对于float类型的获取，我首先对比了i32类型的书写格式，推测到FloatType要像Int32Type一样提前申请，然后翻阅了Type.h文件找到了floattype相关的内容，最终解决了该问题；第二个是为何要进行set_insert_point，首先根据字面意思，猜测是要设置插入点，然后根据例子和核心类说明，大致理解了其功能，相当于在ll文件中的申请一个个独立的BasicBlock然后在他们之间进行跳转。

对于实验的第3部分，本身并不难，难点在于两点。第一点，读懂visit和accept，并且看懂各类的功能及说明，最初我对于accept始终不理解，在通过字面意思的推断以及其返回值为visit后意识到，它的功能和visit等价；第二点，也是最容易出错的一点，对于AddSubNode和MulDivNode应区别来分析，因为AddSubNode先访问右子结点再访问左子结点，而另一者相反。

## 实验反馈
吐槽?建议?

​	在本次实验中，代码部分并不困难，难度主要对于所给实验要求的理解上以及对背景知识的掌握上，通过issue中的经验、助教所提供材料的详细解释、官方文档以及网上的教程，花费了不少时间掌握知识点；其中，大量时间用于理解IR核心类与其所生成的IR语句的逻辑联系，此外，还花费大量的时间阅读理解所给出的每一个函数的作用以及各变量的功能。出现的最主要的错误在于不够细心，以及对于部分内容的理解不够透彻。本次实验中，系统的复习并实践了对于中间代码生成的知识，了解了LLVM IR的使用及其功能，成功实现了助教的实验要求。

​	对于本实验并没有什么吐槽或者建议，但对于未来的实验十分建议助教像这次一样给出一个复杂例子以及详细的注释。读懂例子对我理解相关知识以及上手书写cpp和ll文件真的起到了很大很大的帮助，依葫芦画瓢也快速帮我掌握了语句用法。（类目，梦回当年第一次接触编程的从0到1的日子）