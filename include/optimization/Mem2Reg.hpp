#ifndef SYSYC_MEM2REG_HPP
#define SYSYC_MEM2REG_HPP

#include "Module.h"
#include "Function.h"
#include "IRBuilder.h"
#include "BasicBlock.h"
#include "Instruction.h"
#include "PassManager.hpp"
#include "Dominators.h"

class Mem2Reg : public Pass
{
private:
    Function * func_;
    Dominators *dominators_;

public:
    Mem2Reg(Module *m) : Pass(m){}
    ~Mem2Reg(){};
    void run() override;
    void generate_phi();
    void re_name(BasicBlock *bb);
    void remove_alloca();
};

#endif