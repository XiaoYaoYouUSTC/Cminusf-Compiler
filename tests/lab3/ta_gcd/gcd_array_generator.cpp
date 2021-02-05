#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFP::get(num, module) // 得到常数值的表示,方便后面多次用到

int main() {
  auto module = new Module("Cminus code");  // module name是什么无关紧要
  auto builder = new IRBuilder(nullptr, module);
  Type *Int32Type = Type::get_int32_type(module);

  // 全局数组,x,y
  auto *arrayType = ArrayType::get(Int32Type, 1);
  auto initializer = ConstantZero::get(Int32Type, module);
  auto x = GlobalVariable::create("x", module, arrayType, false, initializer);// 参数解释：  名字name，所属module，全局变量类型type，
  auto y = GlobalVariable::create("y", module, arrayType, false, initializer);//          是否是常量定义（cminus中没有常量概念，应全都是false），初始化常量(ConstantZero类)

  // gcd函数
  // 函数参数类型的vector
  std::vector<Type *> Ints(2, Int32Type);

  //通过返回值类型与参数类型列表得到函数类型
  auto gcdFunTy = FunctionType::get(Int32Type, Ints);

  // 由函数类型得到函数
  auto gcdFun = Function::create(gcdFunTy,
                                 "gcd", module);

  // BB的名字在生成中无所谓,但是可以方便阅读
  auto bb = BasicBlock::create(module, "entry", gcdFun);
  
  builder->set_insert_point(bb);                        // 一个BB的开始,将当前插入指令点的位置设在bb
  
  auto retAlloca = builder->create_alloca(Int32Type);   // 在内存中分配返回值的位置
  auto uAlloca = builder->create_alloca(Int32Type);     // 在内存中分配参数u的位置
  auto vAlloca = builder->create_alloca(Int32Type);     // 在内存中分配参数v的位置 

  std::vector<Value *> args;  // 获取gcd函数的形参,通过Function中的iterator
  for (auto arg = gcdFun->arg_begin(); arg != gcdFun->arg_end(); arg++) {
    args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
  }

  builder->create_store(args[0], uAlloca);  // 将参数u store下来
  builder->create_store(args[1], vAlloca);  // 将参数v store下来

  auto vLoad = builder->create_load(vAlloca);           // 将参数v load上来
  auto icmp = builder->create_icmp_eq(vLoad, CONST_INT(0));  // v和0的比较,注意ICMPEQ

  auto trueBB = BasicBlock::create(module, "trueBB", gcdFun);    // true分支
  auto falseBB = BasicBlock::create(module, "falseBB", gcdFun);  // false分支
  auto retBB = BasicBlock::create(
      module, "", gcdFun);  // return分支,提前create,以便true分支可以br

  auto br = builder->create_cond_br(icmp, trueBB, falseBB);  // 条件BR
  DEBUG_OUTPUT // 我调试的时候故意留下来的,以醒目地提醒你这个调试用的宏定义方法
  builder->set_insert_point(trueBB);  // if true; 分支的开始需要SetInsertPoint设置
  auto uLoad = builder->create_load(uAlloca);
  builder->create_store(uLoad, retAlloca);
  builder->create_br(retBB);  // br retBB

  builder->set_insert_point(falseBB);  // if false
  uLoad = builder->create_load(uAlloca);
  vLoad = builder->create_load(vAlloca);
  auto div = builder->create_isdiv(uLoad, vLoad);  // SDIV - div with S flag
  auto mul = builder->create_imul(div, vLoad);  // MUL - mul
  auto sub = builder->create_isub(uLoad, mul);  // the same
  auto call = builder->create_call(gcdFun, {vLoad, sub}); // 创建call指令
  // {vLoad, sub} - 参数array
  builder->create_store(call, retAlloca);
  builder->create_br(retBB);  // br retBB

  builder->set_insert_point(retBB);  // ret分支
  auto retLoad = builder->create_load(retAlloca);
  builder->create_ret(retLoad);

  // funArray函数
  auto Int32PtrType = Type::get_int32_ptr_type(module);   // 单个参数的类型,指针
  std::vector<Type *> IntPtrs(2, Int32PtrType);           // 参数列表类型
  auto funArrayFunType = FunctionType::get(Int32Type, IntPtrs);   // 函数类型
  auto funArrayFun = Function::create(funArrayFunType, "funArray", module);
  bb = BasicBlock::create(module, "entry", funArrayFun);
  builder->set_insert_point(bb);
  auto upAlloca = builder->create_alloca(Int32PtrType);   // u的存放
  auto vpAlloca = builder->create_alloca(Int32PtrType);   // v的存放
  auto aAlloca = builder->create_alloca(Int32Type);       // a的存放
  auto bAlloca = builder->create_alloca(Int32Type);       // b的存放
  auto tempAlloca = builder->create_alloca(Int32Type);    // temp的存放

  std::vector<Value *> args1;  //获取funArrayFun函数的形参,通过Function中的iterator
  for (auto arg = funArrayFun->arg_begin(); arg != funArrayFun->arg_end(); arg++) {
    args1.push_back(*arg); // * 号运算符是从迭代器中取出迭代器当前指向的元素
  }
  builder->create_store(args1[0], upAlloca);              // 将参数u store下来
  builder->create_store(args1[1], vpAlloca);              // 将参数v store下来

  auto u0pLoad = builder->create_load(upAlloca);          // 读取u
  auto u0GEP = builder->create_gep(u0pLoad, {CONST_INT(0)});  // GEP: 获取u[0]地址
  auto u0Load = builder->create_load(u0GEP);              // 从u[0]地址 读取u[0]
  builder->create_store(u0Load, aAlloca);                 // 将u[0] 写入 a
  auto v0pLoad = builder->create_load(vpAlloca);          // 同上
  auto v0GEP = builder->create_gep(v0pLoad, {CONST_INT(0)});
  auto v0Load = builder->create_load(v0GEP);
  builder->create_store(v0Load, bAlloca);

  auto aLoad = builder->create_load(aAlloca);
  auto bLoad = builder->create_load(bAlloca);
  icmp = builder->create_icmp_lt(aLoad, bLoad);
  trueBB = BasicBlock::create(module, "trueBB", funArrayFun);
  falseBB = BasicBlock::create(module, "falseBB", funArrayFun);
  builder->create_cond_br(icmp, trueBB, falseBB);

  builder->set_insert_point(trueBB);
  builder->create_store(aLoad, tempAlloca);
  builder->create_store(bLoad, aAlloca);
  auto tempLoad = builder->create_load(tempAlloca);
  builder->create_store(tempLoad, bAlloca);
  builder->create_br(falseBB);  // 注意在下一个BB之前要Br一下

  builder->set_insert_point(falseBB);
  aLoad = builder->create_load(aAlloca);
  bLoad = builder->create_load(bAlloca);
  call = builder->create_call(gcdFun, {aLoad, bLoad});
  builder->create_ret(call);


  // main函数
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  bb = BasicBlock::create(module, "entry", mainFun);
  // BasicBlock的名字在生成中无所谓,但是可以方便阅读
  builder->set_insert_point(bb);

  retAlloca = builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0), retAlloca);             // 默认 ret 0

  auto x0GEP = builder->create_gep(x, {CONST_INT(0), CONST_INT(0)});  // GEP: 这里为什么是{0, 0}呢? (实验报告相关)
  builder->create_store(CONST_INT(90), x0GEP);
  auto y0GEP = builder->create_gep(y, {CONST_INT(0), CONST_INT(0)});  // GEP: 这里为什么是{0, 0}呢? (实验报告相关)
  builder->create_store(CONST_INT(18), y0GEP);

  x0GEP = builder->create_gep(x, {CONST_INT(0), CONST_INT(0)});
  y0GEP = builder->create_gep(y, {CONST_INT(0), CONST_INT(0)});
  call = builder->create_call(funArrayFun, {x0GEP, y0GEP});           // 为什么这里传的是{x0GEP, y0GEP}呢?

  builder->create_ret(call);
  // 给这么多注释了,但是可能你们还是会弄很多bug
  // 所以强烈建议配置AutoComplete,效率会大大提高!
  // 别人配了AutoComplete,只花1小时coding
  // 你没有配AutoComplete,找method花5小时,debug花5小时,肯定哭唧唧!
  // 最后,如果猜不到某个IR指令对应的C++的函数,建议把指令翻译成英语然后在method列表中搜索一下
  // 最后的最后,这个例子只涉及到了一点基本的指令生成,
  // 对于额外的指令,包括数组,在之后的实验中可能需要大家好好搜索一下思考一下,
  // 还有涉及到的C++语法,可以在gitlab上发issue提问或者向大家提供指导,会有bonus哦!
  // 对于这个例子里的代码风格/用法,如果有好的建议也欢迎提出!
  std::cout << module->print();
  delete module;
  return 0;
}