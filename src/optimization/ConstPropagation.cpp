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
#ifdef DEBUG
    std::cerr << "instr: " << instr->get_name() << std::endl;
#endif    
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
    
#ifdef DEBUG
  std::cerr << "set def: " << v->get_name() << std::endl;
#endif

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
