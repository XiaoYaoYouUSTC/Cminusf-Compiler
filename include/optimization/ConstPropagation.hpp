#ifndef CONSTPROPAGATION_HPP
#define CONSTPROPAGATION_HPP
#include "PassManager.hpp"
#include "Constant.h"
#include "Instruction.h"
#include "Module.h"

#include "Value.h"
#include "IRBuilder.h"
#include <vector>
#include <stack>
#include <unordered_map>

// tips: 用来判断value是否为ConstantFP/ConstantInt
ConstantFP* cast_constantfp(Value *value);
ConstantInt* cast_constantint(Value *value);


// tips: ConstFloder类

class ConstFolder
{
public:
    ConstFolder(Module *m) : module_(m) {}
    ConstantInt *computeINT(
        Instruction::OpID op,
        ConstantInt *value1,
        ConstantInt *value2);
    ConstantFP *computeFP(
        Instruction::OpID op,
        ConstantFP *value1,
        ConstantFP *value2);
    ConstantInt *computeCMP(
        CmpInst::CmpOp op,
        ConstantInt *value1,
        ConstantInt *value2);
    ConstantInt *computeFCMP(
        FCmpInst::CmpOp op,
        ConstantFP *value1,
        ConstantFP *value2);
    // ...
private:
    Module *module_;
};

class ConstPropagation : public Pass
{
private:
    IRBuilder *builder_;
    std::map<Value *, ConstantInt *> global_def_INT;
    std::map<Value *, ConstantFP *> global_def_FP;
public:
    ConstPropagation(Module *m) : Pass(m) {
        builder_ = new IRBuilder(nullptr,m);
    }
    void run();
    ConstantInt *get_def_val_INT(Value *v);
    ConstantFP *get_def_val_FP(Value *v);
    ConstantInt *set_def_val_INT(Value *v, ConstantInt *const_val);
    ConstantFP *set_def_val_FP(Value *v, ConstantFP *const_val);
};

#endif
