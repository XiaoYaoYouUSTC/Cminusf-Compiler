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
  
    //main function
  auto mainFun = Function::create(FunctionType::get(Int32Type,{}),"main",module);
  //define the basicblock
  auto bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //define ret,a,i
  auto retAlloca = builder->create_alloca(Int32Type);
  auto aAlloca = builder->create_alloca(Int32Type);
  auto iAlloca = builder->create_alloca(Int32Type);
  //ret 0,a 10,i 0
  builder->create_store(CONST_INT(0),retAlloca);
  builder->create_store(CONST_INT(10),aAlloca);
  builder->create_store(CONST_INT(0),iAlloca);
  //compare i with 10 to decide whether to enter the loop
  auto iLoad=builder->create_load(iAlloca);
  auto icmp=builder->create_icmp_lt(iLoad,CONST_INT(10));
   //define the loopBB ,outBB
  auto loopBB = BasicBlock::create(module, "loopBB", mainFun);    // loop分支
  auto outBB = BasicBlock::create(module, "outBB", mainFun);  // out分支
  //decide which BB we should go
  builder->create_cond_br(icmp,loopBB,outBB);
  
  //loop branch
  builder->set_insert_point(loopBB);
  //i=i+1
  iLoad=builder->create_load(iAlloca);
  auto add=builder->create_iadd(iLoad,CONST_INT(1));
  builder->create_store(add,iAlloca);
  //a=a+i
  auto aLoad=builder->create_load(aAlloca);
  iLoad=builder->create_load(iAlloca);
  add=builder->create_iadd(aLoad,iLoad);
  builder->create_store(add,aAlloca);
  //compare i with 10
  iLoad=builder->create_load(iAlloca);
  icmp=builder->create_icmp_lt(iLoad,CONST_INT(10));
  builder->create_cond_br(icmp,loopBB,outBB);
  
  
  //return
  builder->set_insert_point(outBB);
  aLoad=builder->create_load(aAlloca);
  builder->create_ret(aLoad);
  
  
   std::cout<<module->print();
  delete module;
  return 0;
}
  

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
