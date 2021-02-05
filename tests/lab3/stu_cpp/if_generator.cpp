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
  Type *FloatType=Type::get_float_type(module);
  
  //main function
  auto mainFun = Function::create(FunctionType::get(Int32Type,{}),"main",module);
  //define the basicblock
  auto bb = BasicBlock::create(module,"entry",mainFun);
  builder->set_insert_point(bb);
  //ret 
  auto retAlloca=builder->create_alloca(Int32Type);
  //define a
  auto aAlloca=builder->create_alloca(FloatType);
  //ret 0
  builder->create_store(CONST_INT(0),retAlloca);
  //a=5.555
  builder->create_store(CONST_FP(5.555),aAlloca);
  //compare a with 1
  auto aLoad =builder->create_load(aAlloca);
  auto fcmp =builder->create_fcmp_gt(aLoad,CONST_FP(1.0));
  //define the trueBB ,falseBB and retBB
  auto trueBB = BasicBlock::create(module, "trueBB", mainFun);    // true分支
  auto falseBB = BasicBlock::create(module, "falseBB", mainFun);  // false分支
  auto retBB = BasicBlock::create(module, "", mainFun); // return分支
  //decide which BB we should go
  builder->create_cond_br(fcmp,trueBB,falseBB);
  
  //trueBB,ret 233
  builder->set_insert_point(trueBB);
  builder->create_store(CONST_INT(233),retAlloca);
  builder->create_br(retBB);
  
   //falseBB,ret 0
  builder->set_insert_point(falseBB);
  builder->create_store(CONST_INT(0),retAlloca);
  builder->create_br(retBB);
  
  //retBB
  builder->set_insert_point(retBB);
  auto retLoad=builder->create_load(retAlloca);
  builder->create_ret(retLoad);
  
  
  std::cout<<module->print();
  delete module;
  return 0;
}
  
