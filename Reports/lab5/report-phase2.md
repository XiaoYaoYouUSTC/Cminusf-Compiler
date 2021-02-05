# Lab5 实验报告

小组成员 姓名 学号

陈俊驰 PB18000051 (队长)

余卓然 PB18000095
## 实验要求

请按照自己的理解，写明本次实验需要干什么

首先，通过实验第一部分阅读Mem2Reg和LoopSearch两个优化Pass的代码学习的基于LightIR的优化Pass开发的流程和相关知识。然后根据所学知识以及助教所提供的对于各类基本优化Pass的提示与相关知识，利用所提供的API来补全ConstPropagation.cpp、LoopInvHoist.cpp、ActiveVars.cpp等文件，从而实现对于常量传播、循环不变式外提、活跃变量分析等基本优化Pass的开发实现。

## 实验难点

实验中遇到哪些挑战

1.理解各优化的定义，以及如何将定义修改为算法，进而实现该优化；

2.如何利用已有的API，对已经生成好的module进行修改或者获得信息而不影响module中的与优化无关的部分，例如在br前插入代码而不影响各bb间的前后关系；

3.如何正确利用C++所提供的各类数据结构，如set等，及其相关的函数；

4.在常量传播中如何确定一个常量传播的范围以及对于冗余语句的处理；

5.在循环不变式外提中如何正确利用LoopSearch所提供的信息从而进行循环不变式及外提的判定；

6.在活跃变量分析中，如何对于各类IR指令的操作数及结果分情况处理从而获得use及def，此外，还要对现有的数据流方程根据phi进行修改。

## 实验设计

* 常量传播

    实现思路：

    由实验Document可知，常量传播的优化a.只需要考虑过程内的常量传播，不用考虑数组 b.整形、浮点型都要考虑。因此可以对instruction分类讨论，可以分为基本运算类型的instruction、强制类型转换的instr、比较类型的instr等，由于全局变量在块内传播，所以对于load、store指令也需要处理，使用一个map类型存储全局变量，在load时取出map中的值，在store时更新map，每种类型的instr也需要相应进行整形、浮点型的分类讨论。除此之外还需要对br进行处理，删除无效分支并更新块之间的关系

    相应代码：

    ConstPropagation.hpp:

```sh
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

```


ConstPropagation.cpp:

```sh

#include "ConstPropagation.hpp"
#include "logging.hpp"
//#define DEBUG

ConstantInt *ConstFolder::computeINT(
    Instruction::OpID op,
    ConstantInt *value1,
    ConstantInt *value2)
{

    int c_value1 = value1->get_value();
    int c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::add:
        return ConstantInt::get(c_value1 + c_value2, module_);
        break;
    case Instruction::sub:
        return ConstantInt::get(c_value1 - c_value2, module_);
        break;
    case Instruction::mul:
        return ConstantInt::get(c_value1 * c_value2, module_);
        break;
    case Instruction::sdiv:
        return ConstantInt::get((int)(c_value1 / c_value2), module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantInt *ConstFolder::computeCMP(
    CmpInst::CmpOp op, 
    ConstantInt *value1,
    ConstantInt *value2) 
{
    int lhs = value1->get_value();
    int rhs = value2->get_value();
    bool ret;
    switch (op) {
        case CmpInst::EQ:
            ret = (lhs == rhs);
            break;
        case CmpInst::NE:
            ret = (lhs != rhs);
            break;
        case CmpInst::GT:
            ret = (lhs > rhs);
            break;
        case CmpInst::GE:
            ret = (lhs >= rhs);
            break;
        case CmpInst::LE:
            ret = (lhs <= rhs);
            break;
        case CmpInst::LT:
            ret = (lhs < rhs);
            break;
        default:
            return nullptr;
            break;
    }
    return ConstantInt::get(ret, module_);
}

ConstantFP *ConstFolder::computeFP(
    Instruction::OpID op,
    ConstantFP *value1,
    ConstantFP *value2)
{

    float c_value1 = value1->get_value();
    float c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::fadd:
        return ConstantFP::get(c_value1 + c_value2, module_);

        break;
    case Instruction::fsub:
        return ConstantFP::get(c_value1 - c_value2, module_);
        break;
    case Instruction::fmul:
        return ConstantFP::get(c_value1 * c_value2, module_);
        break;
    case Instruction::fdiv:
        return ConstantFP::get(c_value1 / c_value2, module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantInt *ConstFolder::computeFCMP(
    FCmpInst::CmpOp op, 
    ConstantFP *value1,
    ConstantFP *value2) 
{
    float lhs = value1->get_value();
    float rhs = value2->get_value();
    bool ret;
    switch (op) {
        case CmpInst::EQ:
            ret = (lhs == rhs);
            break;
        case CmpInst::NE:
            ret = (lhs != rhs);
            break;
        case CmpInst::GT:
            ret = (lhs > rhs);
            break;
        case CmpInst::GE:
            ret = (lhs >= rhs);
            break;
        case CmpInst::LE:
            ret = (lhs <= rhs);
            break;
        case CmpInst::LT:
            ret = (lhs < rhs);
            break;
        default:
            return nullptr;
            break;
    }
    return ConstantInt::get(ret, module_);
}

// 用来判断value是否为ConstantFP，如果不是则会返回nullptr
ConstantFP *cast_constantfp(Value *value)
{
    auto constant_fp_ptr = dynamic_cast<ConstantFP *>(value);
    if (constant_fp_ptr)
    {
        return constant_fp_ptr;
    }
    else
    {
        return nullptr;
    }
}
ConstantInt *cast_constantint(Value *value)
{
    auto constant_int_ptr = dynamic_cast<ConstantInt *>(value);
    if (constant_int_ptr)
    {
        return constant_int_ptr;
    }
    else
    {
        return nullptr;
    }
}


void ConstPropagation::run()
{
    std::vector<Instruction *> wait_delete;
    ConstFolder *fold = new ConstFolder(m_);
    global_def_INT.clear();
    global_def_FP.clear();
    for (auto func:m_->get_functions()){
        for (auto bb:func->get_basic_blocks()){
            global_def_INT.clear();
            global_def_FP.clear();
            for (auto instr:bb->get_instructions()){
   
                if(instr->is_add()||instr->is_sub()||instr->is_mul()||instr->is_div()||instr->is_fadd()||instr->is_fsub()||instr->is_fmul()||instr->is_fdiv()){
                    auto vi1 = cast_constantint(instr->get_operand(0));
                    auto vi2 = cast_constantint(instr->get_operand(1));
                    auto vf1 = cast_constantfp(instr->get_operand(0));
                    auto vf2 = cast_constantfp(instr->get_operand(1));
                    
                    if(vi1&&vi2){
                        auto fold_const = fold->computeINT(instr->get_instr_type(),vi1, vi2);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, fold_const);
                        }
                        wait_delete.push_back(instr);
                    }

                    else if((vi1||vf1)&&(vi2||vf2)){
                        if(vi1)
                            vf1 = dynamic_cast<ConstantFP *>(vi1);
                        if(vi2)
                            vf2 = dynamic_cast<ConstantFP *>(vi2);
                        auto fold_const = fold->computeFP(instr->get_instr_type(), vf1, vf2);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, fold_const);
                        }
                        wait_delete.push_back(instr);
                    }
                    
                }
                
                else if(instr->is_fp2si()){
                    auto v = cast_constantfp(instr->get_operand(0));
                    if(v){    
                        int tmp = (int)(v->get_value());
                        auto t = ConstantInt::get(tmp,m_);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_,t);
                        }
                        wait_delete.push_back(instr);
                    }
                    
                }

                else if(instr->is_si2fp()){
                    auto v = cast_constantint(instr->get_operand(0));
                    if(v){    
                        float tmp = (float)(v->get_value());
                        auto t = ConstantFP::get(tmp,m_);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_,t);
                        }
                        wait_delete.push_back(instr);
                    }
                    
                }
                
                else if(instr->is_cmp()){
                    auto vi1 = cast_constantint(instr->get_operand(0));
                    auto vi2 = cast_constantint(instr->get_operand(1));
                    if(vi1&&vi2){
                        auto fold_const = fold->computeCMP(dynamic_cast<CmpInst *>(instr)->get_cmp_op(),vi1, vi2);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, fold_const);
                        }
                        wait_delete.push_back(instr);
                    }
                }
                
                else if(instr->is_fcmp()){
                    auto vf1 = cast_constantfp(instr->get_operand(0));
                    auto vf2 = cast_constantfp(instr->get_operand(1));
                    if(vf1&&vf2){
                        auto fold_const = fold->computeFCMP(dynamic_cast<FCmpInst *>(instr)->get_cmp_op(),vf1, vf2);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, fold_const);
                        }
                        wait_delete.push_back(instr);
                    }
                }
                
                else if(instr->is_zext()){
                    auto v = cast_constantint(instr->get_operand(0));
                    if(v){    
                        int tmp = v->get_value();
                        auto t = ConstantInt::get(tmp,m_);
                        for (auto use:instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_,t);
                        }
                        wait_delete.push_back(instr);
                    }
                }

                else if(instr->is_load()){
                    auto vi1 = get_def_val_INT(instr->get_operand(0));
                    auto vf1 = get_def_val_FP(instr->get_operand(0));
                    if (vi1) {
                        for (auto use : instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, vi1);
                        }
                        wait_delete.push_back(instr);
                    }
                    else if(vf1){
                        for (auto use : instr->get_use_list()) {
                            dynamic_cast<User *>(use.val_)->set_operand(use.arg_no_, vf1);
                        }
                        wait_delete.push_back(instr);
                    }
                }
                
                else if(instr->is_store()){
                    auto v = dynamic_cast<StoreInst *>(instr)->get_lval();
                    auto vi1 = cast_constantint(dynamic_cast<StoreInst *>(instr)->get_rval());
                    auto vf1 = cast_constantfp(dynamic_cast<StoreInst *>(instr)->get_rval());
                    if(vi1||vf1){
                        if(vi1)
                            set_def_val_INT(v,vi1);
                        else if(vf1)
                            set_def_val_FP(v,vf1);
                    }
                    else if(global_def_INT.find(v)!=global_def_INT.end()){
                        global_def_INT.erase(global_def_INT.find(v));
                        //global_def_FP.erase(global_def_FP.find(v));
                    }
                    else if(global_def_FP.find(v)!=global_def_FP.end()){
                        //global_def_INT.erase(global_def_INT.find(v));
                        global_def_FP.erase(global_def_FP.find(v));
                    }
                }
                
                
            }
            
            for(auto instr:wait_delete){
                bb->delete_instr(instr);
            }
            
        }
    }
    
    for (auto func:m_->get_functions()){
        for (auto bb:func->get_basic_blocks()){
            builder_->set_insert_point(bb);
            if (bb->get_terminator()->is_br()){
                
                auto br = bb->get_terminator();
                if (dynamic_cast<BranchInst *>(br)->is_cond_br()) {
                    auto cond = cast_constantint(br->get_operand(0));
                    auto truebb = br->get_operand(1);
                    auto falsebb = br->get_operand(2);
                    if(cond){
                        if(cond->get_value()==0){
                            bb->delete_instr(br);
                            for (auto succ_bb : bb->get_succ_basic_blocks()) {
                                succ_bb->remove_pre_basic_block(bb);
                                if (succ_bb != falsebb) {
                                    for (auto instr : succ_bb->get_instructions()) {
                                        if (instr->is_phi()) {
                                            for (int i = 0; i < instr->get_num_operand(); i++) {
                                                if (i % 2 == 1) {
                                                    if (instr->get_operand(i) == bb) {
                                                        instr->remove_operands(i - 1, i);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            builder_->create_br(dynamic_cast<BasicBlock *>(falsebb));
                            bb->get_succ_basic_blocks().clear();
                            bb->add_succ_basic_block(dynamic_cast<BasicBlock *>(falsebb));
                        } 
                        
                        else {
                            bb->delete_instr(br);
                            
                            for (auto succ_bb : bb->get_succ_basic_blocks()) {
                                succ_bb->remove_pre_basic_block(bb);
                                if (succ_bb != truebb) {
                                    for (auto instr : succ_bb->get_instructions()) {
                                        if (instr->is_phi()) {
                                            for (int i = 0; i < instr->get_num_operand(); i++) {
                                                if (i % 2 == 1) {
                                                    if (instr->get_operand(i) == bb) {
                                                        instr->remove_operands(i - 1, i);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            builder_->create_br(dynamic_cast<BasicBlock *>(truebb));
                            bb->get_succ_basic_blocks().clear();
                            bb->add_succ_basic_block(dynamic_cast<BasicBlock *>(truebb));
                            
                        }
                        
                    }
                }

            }
        }
    }
    return ;
    
}


ConstantInt *ConstPropagation::get_def_val_INT (Value *v) {
    auto global_v = dynamic_cast<GlobalVariable *>(v);

    if (global_v != nullptr) {
        if (global_def_INT.find(global_v) != global_def_INT.end()) {
            auto iter = global_def_INT.find(global_v);
            return iter->second;
        } 
        else 
            return nullptr;
    } 
    return nullptr;
}

ConstantFP *ConstPropagation::get_def_val_FP (Value *v) {
    auto global_v = dynamic_cast<GlobalVariable *>(v);

    if (global_v != nullptr) {
        if (global_def_FP.find(v) != global_def_FP.end()) {
            auto iter = global_def_FP.find(v);
            return iter->second;
        } 
        else 
            return nullptr;
    } 
    return nullptr;
}

ConstantInt *ConstPropagation::set_def_val_INT(Value *v, ConstantInt *const_val) {
    

    auto global_v = dynamic_cast<GlobalVariable *>(v);

    if (global_v != nullptr) {
        if (global_def_INT.find(global_v) != global_def_INT.end()) {
            auto iter = global_def_INT.find(global_v);
            iter->second = const_val;
            return iter->second;
        } 
        else {
            //global_def_INT.insert({global_v, const_val});
            global_def_INT[global_v] = const_val;
            return global_def_INT.find(global_v)->second;
        }
    }
    return nullptr;
}

ConstantFP *ConstPropagation::set_def_val_FP(Value *v, ConstantFP *const_val) {

    auto global_v = dynamic_cast<GlobalVariable *>(v);
    if (global_v != nullptr) {
        if (global_def_FP.find(global_v) != global_def_FP.end()) {
            auto iter = global_def_FP.find(global_v);
            iter->second = const_val;
            return iter->second;
        } 
        else {
            global_def_FP[global_v] = const_val;
            //global_def_FP.insert({global_v, const_val});
            return global_def_FP.find(global_v)->second;
        }
    }
    return nullptr;
}


```

    优化前后的IR对比（举一个例子）并辅以简单说明：
      
      优化前：

```sh

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  br label %label2
label2:                                                ; preds = %label_entry, %label7
  %op78 = phi i32 [ 0, %label_entry ], [ %op73, %label7 ]
  %op79 = phi i32 [ 0, %label_entry ], [ %op40, %label7 ]
  %op4 = icmp slt i32 %op78, 100000000
  %op5 = zext i1 %op4 to i32
  %op6 = icmp ne i32 %op5, 0
  br i1 %op6, label %label7, label %label74
label7:                                                ; preds = %label2
  %op8 = add i32 1, 1
  %op9 = add i32 %op8, 1
  %op10 = add i32 %op9, 1
  %op11 = add i32 %op10, 1
  %op12 = add i32 %op11, 1
  %op13 = add i32 %op12, 1
  %op14 = add i32 %op13, 1
  %op15 = add i32 %op14, 1
  %op16 = add i32 %op15, 1
  %op17 = add i32 %op16, 1
  %op18 = add i32 %op17, 1
  %op19 = add i32 %op18, 1
  %op20 = add i32 %op19, 1
  %op21 = add i32 %op20, 1
  %op22 = add i32 %op21, 1
  %op23 = add i32 %op22, 1
  %op24 = add i32 %op23, 1
  %op25 = add i32 %op24, 1
  %op26 = add i32 %op25, 1
  %op27 = add i32 %op26, 1
  %op28 = add i32 %op27, 1
  %op29 = add i32 %op28, 1
  %op30 = add i32 %op29, 1
  %op31 = add i32 %op30, 1
  %op32 = add i32 %op31, 1
  %op33 = add i32 %op32, 1
  %op34 = add i32 %op33, 1
  %op35 = add i32 %op34, 1
  %op36 = add i32 %op35, 1
  %op37 = add i32 %op36, 1
  %op38 = add i32 %op37, 1
  %op39 = add i32 %op38, 1
  %op40 = add i32 %op39, 1
  %op44 = mul i32 %op40, %op40
  %op46 = mul i32 %op44, %op40
  %op48 = mul i32 %op46, %op40
  %op50 = mul i32 %op48, %op40
  %op52 = mul i32 %op50, %op40
  %op54 = mul i32 %op52, %op40
  %op56 = mul i32 %op54, %op40
  %op59 = mul i32 %op40, %op40
  %op61 = mul i32 %op59, %op40
  %op63 = mul i32 %op61, %op40
  %op65 = mul i32 %op63, %op40
  %op67 = mul i32 %op65, %op40
  %op69 = mul i32 %op67, %op40
  %op71 = mul i32 %op69, %op40
  %op72 = sdiv i32 %op56, %op71
  %op73 = add i32 %op78, %op72
  br label %label2
label74:                                                ; preds = %label2
  %op77 = mul i32 %op79, %op79
  call void @output(i32 %op77)
  ret void
}

```

优化后： 

```sh

; ModuleID = 'cminus'
source_filename = "/home/haiqwa/2020fall-compiler_cminus/tests/lab5/./testcases/ConstPropagation/testcase-1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  br label %label2
label2:                                                ; preds = %label_entry, %label7
  %op78 = phi i32 [ 0, %label_entry ], [ %op73, %label7 ]
  %op79 = phi i32 [ 0, %label_entry ], [ 34, %label7 ]
  %op4 = icmp slt i32 %op78, 100000000
  %op5 = zext i1 %op4 to i32
  %op6 = icmp ne i32 %op5, 0
  br i1 %op6, label %label7, label %label74
label7:                                                ; preds = %label2
  %op73 = add i32 %op78, 1
  br label %label2
label74:                                                ; preds = %label2
  %op77 = mul i32 %op79, %op79
  call void @output(i32 %op77)
  ret void
}

```


* 循环不变式外提

    实现思路：

    通过逻辑推理可知，如果一个式子对于内层循环为循环不变式，对于外层循环不是，则无论先对内层还是外层循环外提循环不变式，该式仅会外提到内层循环外且外层循环中；如果对于内层循环为循环不变式，对于外层循环也是，则无论何种顺序访问内外循环，都会将该式提到外层循环外。故仅需遍历所有循环一次，便可以将所有循环不变式外提完成。对于每一个循环，首先遍历循环中的所有bb的所有指令，将其所赋值的变量加入change_var集合中，利用拓扑排序的思想，依次来找到操作数不包括这些在该循环中被赋值的变量作为操作数的指令并存入待删除指令序列delete_instr中，并将该指令的所进行的赋值从change_var中删除，重复进行如上操作直至change_var及delete_instr不再发生变化。从而实现找到循环不变式。此时，遍历循环base地址的所有前驱，理论上仅两个，找到不在loop中的那一个prebb。将prebb最后的终结指令从指令表取出，然后在指令表中插入delete_instr中的所有指令，在讲终结指令存回。最后对于delete_instr中的每一个指令，找到它原本所在的块，并将待删除的指令从块的指令链中移除，注意不是进行删除。从而实现外提。

    相应代码：

    对于LoopInvHoist.hpp没有进行改动，仅修改了LoopInvHoist.cpp，具体代码如下

    ```sh
    void LoopInvHoist::run()
    {
        // 先通过LoopSearch获取循环的相关信息
        LoopSearch loop_searcher(m_, false);
        loop_searcher.run();

        // 接下来由你来补充啦！
        int change_flag=0;
        for(auto f:m_->get_functions())
        {
            for(auto loop:loop_searcher.get_loops_in_func(f))
            {
                change_flag=1;
                std::set<Value*> change_var;
                std::vector<Instruction*> delete_instr;
                for(auto bb:(*loop))
                {
                    for(auto instr:bb->get_instructions())
                    {   
                        if(instr->isBinary()||instr->is_cmp()||instr->is_fcmp()||instr->is_call()||instr->is_fp2si()||instr->is_si2fp()||instr->is_zext())
                            change_var.insert(instr);
                        if(instr->is_phi())
                            change_var.insert(instr);
                    }  
                }
                //首先将一个循环中所有被赋值的左值加入，这样用来找到没被改变的
                while(change_flag==1)
                {
                    change_flag=0;
                    for(auto bb:*loop)
                    {
                        for(auto instr:bb->get_instructions())
                        {//对于循环中的每一条指令
                            if(instr->isBinary()||instr->is_cmp()||instr->is_fcmp()) 
                            {//如果是一条运算，且操作数都不在change里，外提
                                auto op0=instr->get_operand(0);
                                auto op1=instr->get_operand(1);
                                bool iop0=(change_var.find(op0)==change_var.end());
                                bool iop1=(change_var.find(op1)==change_var.end());
                                bool varfind=(change_var.find(instr)!=change_var.end());
                                if(iop0&&iop1&&varfind)
                                {
                                    change_flag=1;
                                    delete_instr.push_back(instr);
                                    change_var.erase(instr);
                                }
                            }
                            if(instr->is_zext()||instr->is_fp2si()||instr->is_si2fp()) 
                            {//如果是一条运算，且操作数都不在change里，外提
                                auto op0=instr->get_operand(0);
                                bool iop0=(change_var.find(op0)==change_var.end());
                                bool varfind=(change_var.find(instr)!=change_var.end());
                                if(iop0&&varfind)
                                {
                                    change_flag=1;
                                    delete_instr.push_back(instr);
                                    change_var.erase(instr);
                                }
                            }
                            if(instr->is_call())
                            {
                                bool needdel=1;
                                for(auto op:instr->get_operands())
                                {
                                    if(change_var.find(op)!=change_var.end())
                                    {
                                        needdel=0;
                                        break;
                                    }
                                }
                                bool varfind=(change_var.find(instr)!=change_var.end());
                                if(needdel&&varfind)
                                {
                                    change_flag=1;
                                    delete_instr.push_back(instr);
                                    change_var.erase(instr);
                                }
                            }
                        }
                    }
                }           
                auto base=loop_searcher.get_loop_base(loop);
                BasicBlock* preBB;
                for(auto pre:base->get_pre_basic_blocks())
                {
                    if((*loop).find(pre)==(*loop).end())
                    {     
                        preBB=pre;
                        break;
                    }
                }
                BasicBlock* homeBB;
                //Instruction* instr;
                Instruction* term;
                term=preBB->get_instructions().back();
                preBB->get_instructions().pop_back();
                for(auto instr:delete_instr)
                {   
                    //instr=delete_instr.back();
                    //delete_instr.pop_back();
                    preBB->add_instruction(instr);
                    for(auto bb:(*loop))
                    {
                        for(auto homeinstr:bb->get_instructions())
                        {
                            if(homeinstr==instr)
                                homeBB=bb;
                        }
                    }
                    homeBB->get_instructions().remove(instr);
                }
                preBB->get_instructions().push_back(term);
                delete_instr.clear();
                change_var.clear();
            }
        }
    }
    ```

    优化前后的IR对比（举一个例子）并辅以简单说明：
    
    对于助教所提供的testcase-1，优化前的IR为

    ```sh
    ; ModuleID = 'cminus'
    source_filename = "testcase-1.cminus"

    declare i32 @input()

    declare void @output(i32)

    declare void @outputFloat(float)

    declare void @neg_idx_except()

    define void @main() {
    label_entry:
    br label %label3
    label3:                                                ; preds = %label_entry, %label58
    %op61 = phi i32 [ %op64, %label58 ], [ undef, %label_entry ]
    %op62 = phi i32 [ 1, %label_entry ], [ %op60, %label58 ]
    %op63 = phi i32 [ %op65, %label58 ], [ undef, %label_entry ]
    %op5 = icmp slt i32 %op62, 10000
    %op6 = zext i1 %op5 to i32
    %op7 = icmp ne i32 %op6, 0
    br i1 %op7, label %label8, label %label9
    label8:                                                ; preds = %label3
    br label %label11
    label9:                                                ; preds = %label3
    call void @output(i32 %op61)
    ret void
    label11:                                                ; preds = %label8, %label16
    %op64 = phi i32 [ %op61, %label8 ], [ %op55, %label16 ]
    %op65 = phi i32 [ 0, %label8 ], [ %op57, %label16 ]
    %op13 = icmp slt i32 %op65, 10000
    %op14 = zext i1 %op13 to i32
    %op15 = icmp ne i32 %op14, 0
    br i1 %op15, label %label16, label %label58
    label16:                                                ; preds = %label11
    %op19 = mul i32 %op62, %op62
    %op21 = mul i32 %op19, %op62
    %op23 = mul i32 %op21, %op62
    %op25 = mul i32 %op23, %op62
    %op27 = mul i32 %op25, %op62
    %op29 = mul i32 %op27, %op62
    %op31 = mul i32 %op29, %op62
    %op33 = mul i32 %op31, %op62
    %op35 = mul i32 %op33, %op62
    %op37 = sdiv i32 %op35, %op62
    %op39 = sdiv i32 %op37, %op62
    %op41 = sdiv i32 %op39, %op62
    %op43 = sdiv i32 %op41, %op62
    %op45 = sdiv i32 %op43, %op62
    %op47 = sdiv i32 %op45, %op62
    %op49 = sdiv i32 %op47, %op62
    %op51 = sdiv i32 %op49, %op62
    %op53 = sdiv i32 %op51, %op62
    %op55 = sdiv i32 %op53, %op62
    %op57 = add i32 %op65, 1
    br label %label11
    label58:                                                ; preds = %label11
    %op60 = add i32 %op62, 1
    br label %label3
    }
    ```
    优化后的结果为

    ```sh
    declare i32 @input()

    declare void @output(i32)

    declare void @outputFloat(float)

    declare void @neg_idx_except()

    define void @main() {
    label_entry:
    br label %label3
    label3:                                                ; preds = %label_entry, %label58
    %op61 = phi i32 [ %op64, %label58 ], [ undef, %label_entry ]
    %op62 = phi i32 [ 1, %label_entry ], [ %op60, %label58 ]
    %op63 = phi i32 [ %op65, %label58 ], [ undef, %label_entry ]
    %op5 = icmp slt i32 %op62, 10000
    %op6 = zext i1 %op5 to i32
    %op7 = icmp ne i32 %op6, 0
    br i1 %op7, label %label8, label %label9
    label8:                                                ; preds = %label3
    %op19 = mul i32 %op62, %op62
    %op21 = mul i32 %op19, %op62
    %op23 = mul i32 %op21, %op62
    %op25 = mul i32 %op23, %op62
    %op27 = mul i32 %op25, %op62
    %op29 = mul i32 %op27, %op62
    %op31 = mul i32 %op29, %op62
    %op33 = mul i32 %op31, %op62
    %op35 = mul i32 %op33, %op62
    %op37 = sdiv i32 %op35, %op62
    %op39 = sdiv i32 %op37, %op62
    %op41 = sdiv i32 %op39, %op62
    %op43 = sdiv i32 %op41, %op62
    %op45 = sdiv i32 %op43, %op62
    %op47 = sdiv i32 %op45, %op62
    %op49 = sdiv i32 %op47, %op62
    %op51 = sdiv i32 %op49, %op62
    %op53 = sdiv i32 %op51, %op62
    %op55 = sdiv i32 %op53, %op62
    br label %label11
    label9:                                                ; preds = %label3
    call void @output(i32 %op61)
    ret void
    label11:                                                ; preds = %label8, %label16
    %op64 = phi i32 [ %op61, %label8 ], [ %op55, %label16 ]
    %op65 = phi i32 [ 0, %label8 ], [ %op57, %label16 ]
    %op13 = icmp slt i32 %op65, 10000
    %op14 = zext i1 %op13 to i32
    %op15 = icmp ne i32 %op14, 0
    br i1 %op15, label %label16, label %label58
    label16:                                                ; preds = %label11
    %op57 = add i32 %op65, 1
    br label %label11
    label58:                                                ; preds = %label11
    %op60 = add i32 %op62, 1
    br label %label3
    }
    ```

    对比两个IR可以发现，优化后的IR将从%19到%55的乘除计算，对应于testcase-1.cminus中的(i\*i\*i\*i\*i\*i\*i\*i\*i\*i)/i/i/i/i/i/i/i/i/i/i，根据循环不变式的定义，从对于内层循环的label12，外提到内层循环外外层循环中的label8，其余不变。根据循环不变式的定义从cminus文件中我们可以发现，该式对于内层循环，由于i保持不变，为循环不变式，对于外层循环，由于每次i会加1，故不是循环不变式，外提位置正确。


* 活跃变量分析

    实现思路：

    首先，对于一个函数中每一个bb的每一条指令进行遍历，先访问每一个操作数，如果是未被定值的变量加入bbuse中该bb对应的集合中，然后将该指令所定值的那个变量加入bbdef中bb对应的集合里，其中对于一些特殊的指令例如phi等要分情况讨论，以保证仅将变量加入，避免误将常数等加入use和def集合。由此，可以获得每一个bb的use与def集合，然后利用课本285页的算法循环遍历每一个bb。由于phi的特殊性，不再最直接将OUT置为后继块IN集合的并，而是要将所有后继块的In集合减去phi中不对应该bb的变量的活跃。此后，由于静态单赋值的特点且phi中的值必然来自其他块，一旦再出现则恢复对该bb的活跃，故再去检索该后继块每一个操作数是否包含刚才删去的变量，如果有，那么对于该bb和该后继，该变量仍应活跃，撤销对于该变量的删除。最后将对于每一个后继块上述处理后的集合求并作为OUT，然后将其IN置为useU(out-def)，一旦任意一个IN出现变化，则重复上述操作。

    中间的对于phi的处理可能语言描述较难理解，这里举个例子。

    %op32 = phi i32 [ %op31, %label13 ], [ %op18, %label27 ]

    %op23 = add i32 %op32, %op18

    假设当前的bb为label13，其中一个后继开始的指令如上，首先对于该后继的IN来说，op31、op18都应是活跃的。但对于label13，由于phi的存在，发现op18对应label27，先将其活跃删除，然后对于op23求和指令，由于此时又使用了op18，则再恢复op18的活跃性，故此时对于label13，op31、op18均是活跃的。

    相应的代码：
    ```
    void ActiveVars::run()
    {
        std::ofstream output_active_vars;
        output_active_vars.open("active_vars.json", std::ios::out);
        output_active_vars << "[";
        for (auto &func : this->m_->get_functions()) {
            if (func->get_basic_blocks().empty()) {
                continue;
            }
            else
            {
                func_ = func;  

                func_->set_instr_name();
                live_in.clear();
                live_out.clear();
                
                // 在此分析 func_ 的每个bb块的活跃变量，并存储在 live_in live_out 结构内
                /*for(auto bb:func->get_basic_blocks())
                {
                    std::set<Value *> in_set;
                    live_in.insert({bb,in_set});
                }*/
                std::map<BasicBlock *, std::set<Value *>> bbuse,bbdef;
                for(auto bb:func->get_basic_blocks())
                {
                    for(auto instr:bb->get_instructions())
                        {
                            //常数判断，是否在定义文件里
                            if(instr->is_phi())
                            {
                                for (int i = 0; i < instr->get_num_operand()/2; i++)
                                {
                                    auto op=instr->get_operand(2*i);
                                    if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                    {
                                        if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                    }
                                }
                            }
                            else if(instr->is_store())
                            {
                                auto op=instr->get_operand(0);
                                if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                {
                                    if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                }
                            }
                            else if(instr->is_alloca());
                            else if(instr->is_br())
                            {
                                auto brstr=dynamic_cast<BranchInst*>(instr);
                                if(brstr->is_cond_br())
                                {
                                    auto op=instr->get_operand(0);
                                    if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                    {
                                        if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                    }  
                                }
                            }
                            else if(instr->is_fp2si()||instr->is_fp2si()||instr->is_zext())
                            {
                                auto op=instr->get_operand(0);
                                if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                {
                                    if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                }
                            }
                            else if(instr->is_call())
                            {
                                Value* op;
                                for(int i=1;i<instr->get_num_operand();i++)
                                {
                                    op=instr->get_operand(i);
                                    if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                    {
                                        if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                for(auto op:instr->get_operands())
                                {
                                    if(!(dynamic_cast<ConstantFP *>(op)||dynamic_cast<ConstantInt *>(op)))
                                    {
                                        if(bbdef[bb].find(op)==bbdef[bb].end()&&bbuse[bb].find(op)==bbuse[bb].end())
                                        {
                                            bbuse[bb].insert(op);
                                        }
                                    }
                                }
                            }
                            
                            if(instr->is_void()||instr->is_ret()||instr->is_br());
                            else if(instr->is_store())
                            {
                                if(bbdef[bb].find(instr->get_operand(1))==bbdef[bb].end())
                                    bbdef[bb].insert(instr->get_operand(1));
                            } 
                            else if(instr->is_call())
                            {
                                if(!(instr->is_void()))
                                    if(bbdef[bb].find(instr)==bbdef[bb].end())
                                        bbdef[bb].insert(instr); 
                            }
                            else 
                            {
                                if(bbdef[bb].find(instr)==bbdef[bb].end())
                                    bbdef[bb].insert(instr); 
                            }
                        }
                    /*for(auto useitem:bbuse[bb])
                            {
                                output_active_vars<<"!";
                                output_active_vars << useitem->get_name();
                                output_active_vars<<"!";
                            }
                    for(auto defitem:bbdef[bb])
                            {
                                output_active_vars<<"!";
                                output_active_vars << defitem->get_name();
                                output_active_vars<<"!";
                            }*/
                }
                int flag=1;
                while(flag==1)
                {
                    flag=0;
                    for(auto bb:func->get_basic_blocks())
                    {
                        //output_active_vars << bb->get_name();
                        //output_active_vars << '\n';
                        std::set<Value *> in,out,sucin;
                        for(auto sucbb:bb->get_succ_basic_blocks())
                        {
                            //output_active_vars << sucbb->get_name();
                            for(auto item:live_in[sucbb])
                            {
                                sucin.insert(item);
                            }
                            std::set<Value*>delphiop;
                            for(auto sucinstr:sucbb->get_instructions())
                            {//删除错误的phi的操作数的活跃
                                if(sucinstr->is_phi())
                                {
                                    for(int i=0;i<sucinstr->get_num_operand()/2;i++)
                                        if(sucinstr->get_operand(2*i+1)!=bb)
                                            if(sucin.find(sucinstr->get_operand(2*i))!=sucin.end())
                                            {
                                                sucin.erase(sucinstr->get_operand(2*i));
                                                if(delphiop.find(sucinstr->get_operand(2*i))==delphiop.end())
                                                {
                                                    delphiop.insert(sucinstr->get_operand(2*i));
                                                }
                                            }
                                }
                            }
                            for(auto instr:sucbb->get_instructions())
                            {//某些被删除的phi的操作数将作为其他的活跃再被加回来
                                if(instr->is_phi())
                                {
                                    for (int i = 0; i < instr->get_num_operand()/2; i++)
                                    {
                                        auto op=instr->get_operand(2*i);
                                        if(instr->get_operand(2*i+1)==bb&&delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }                                   
                                    }
                                }
                                else if(instr->is_store())
                                {
                                    auto op=instr->get_operand(0);
                                    if(delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }
                                }
                                else if(instr->is_alloca());
                                else if(instr->is_br())
                                {
                                    auto brstr=dynamic_cast<BranchInst*>(instr);
                                    if(brstr->is_cond_br())
                                    {
                                        auto op=instr->get_operand(0);
                                        if(delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }  
                                    }
                                }
                                else if(instr->is_fp2si()||instr->is_fp2si()||instr->is_zext())
                                {
                                    auto op=instr->get_operand(0);
                                    if(delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }
                                }
                                else if(instr->is_call())
                                {
                                    Value* op;
                                    for(int i=1;i<instr->get_num_operand();i++)
                                    {
                                        op=instr->get_operand(i);
                                        if(delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }
                                    }
                                }
                                else
                                {
                                    for(auto op:instr->get_operands())
                                    {
                                        if(delphiop.find(op)!=delphiop.end())                
                                            if (sucin.find(op)==sucin.end())
                                            {
                                                sucin.insert(op);
                                            }
                                    }
                                }
                            }
                            for(auto item:sucin)
                            {
                                if(out.find(item)==out.end())
                                {
                                    out.insert(item);
                                }
                                if(in.find(item)==in.end())
                                {
                                    in.insert(item);
                                }
                            }
                            sucin.clear();  
                        }
                        /*for(auto item:out)
                            {
                                output_active_vars<<"!";
                                output_active_vars <<item->get_name();
                                output_active_vars<<"!";
                            }*/
                        for(auto defitem:bbdef[bb])
                        {
                            if(in.find(defitem)!=in.end())
                            {
                                in.erase(defitem);
                            }
                        }
                        for(auto useitem:bbuse[bb])
                        {
                            if(in.find(useitem)==in.end())
                            {
                                in.insert(useitem);
                            }
                        }
                        /*if(live_out[bb].size()!=out.size())
                            flag=1;
                        else
                        {
                            for(auto item:out)
                                if(live_out[bb].find(item)==live_out[bb].end())
                                {
                                    flag=1;
                                    break;
                                }
                        }*/
                        if(live_in[bb].size()!=in.size())
                            flag=1;
                        else
                        {
                            for(auto item:in)
                                if(live_in[bb].find(item)==live_in[bb].end())
                                {
                                    flag=1;
                                    break;
                                }
                        }
                        live_out[bb].clear();
                        live_in[bb].clear();
                        for(auto outitem:out)
                        {
                            live_out[bb].insert(outitem);
                        }
                        for(auto initem:in)
                        {
                            live_in[bb].insert(initem);
                        }    
                    }
                }
                output_active_vars << print();
                output_active_vars << ",";
            }
        }
        output_active_vars << "]";
        output_active_vars.close();
        return ;
    }

    ```
### 实验总结

此次实验有什么收获

在本次实验中，主要难点在于理解各优化的定义以及将优化定义转化为算法的处理，在实验实施之前查阅了较多资料，根据已有的知识以及助教提供的文档代码进行学习，和同组成员交流了各种优化的实现思路以及API的使用经验。

在本次实验中，大大地加深了对于课本上的优化的理论知识的理解，并将其运用到了实际操作中，通过对于优化的结果的阅读分析对比，也更加直观的意识到了优化这一操作对于编译的重要意义。同时，由于在优化实现过程中有着大量的对于各类指令的分情况讨论的出现，从而大大加深了对于LightIR及其使用的理解。从广泛运用的知识学习的角度来看，本次实验中的一大收获在于熟练掌握理解了C++中各类数据结构及其相关使用，对于实验中的一些操作起到了化简的作用。同时，由于实验中出现的越界等现象，也意识到这些数据结构的局限性。

在debug过程中尝试多种思路，集思广益发现了较多改进点。一方面，发现了许多个人写代码时发现不了的坏习惯以及不细心导致的错误；另一方面，在本次合作实验中，深刻的理解到了不同视角看问题的优越性，许多小细节的处理、bug的订正方式均是靠着大量的不同理解角度讨论获得的。在这次实验中真正的意识到了团队合作并不在于简单地分工提高效率，而在于讨论中的相互启发，三个臭皮匠胜过诸葛亮，今后应着重进行讨论，分享思路。

### 实验反馈 （可选 不会评分）

对本次实验的建议

十分建议助教像这次一样给出一个复杂例子以及详细的注释，对我理解相关知识以及上手书写起到了很大很大的帮助，依葫芦画瓢也快速帮我掌握了语句用法。

### 组间交流 （可选）

本次实验和哪些组（记录组长学号）交流了哪一部分信息
