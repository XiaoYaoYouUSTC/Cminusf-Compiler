#include "Value.h"
#include "Module.h"
#include "Function.h"
#include "GlobalVariable.h"
#include "Constant.h"
#include "BasicBlock.h"
#include "Instruction.h"
#include "User.h"
#include "Type.h"

std::string print_as_op( Value *v, bool print_ty );
std::string print_cmp_type( CmpInst::CmpOp op);
std::string print_fcmp_type( FCmpInst::CmpOp op);