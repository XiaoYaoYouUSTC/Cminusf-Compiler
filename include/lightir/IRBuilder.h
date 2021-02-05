#ifndef SYSYC_IRBUILDER_H
#define SYSYC_IRBUILDER_H

#include "BasicBlock.h"
#include "Instruction.h"
#include "Value.h"

class IRBuilder {
private:
    BasicBlock *BB_; 
    Module *m_;
public:
    IRBuilder(BasicBlock *bb, Module *m) : BB_(bb), m_(m) {};
    ~IRBuilder() = default;
    Module *get_module(){return m_;}
    BasicBlock *get_insert_block() { return this->BB_; }
    void set_insert_point(BasicBlock *bb) { this->BB_ = bb; } //在某个基本块中插入指令
    BinaryInst *create_iadd( Value *lhs, Value *rhs){ return BinaryInst::create_add( lhs, rhs, this->BB_, m_);}   //创建加法指令（以及其他算术指令）
    BinaryInst *create_isub( Value *lhs, Value *rhs){ return BinaryInst::create_sub( lhs, rhs, this->BB_, m_);}
    BinaryInst *create_imul( Value *lhs, Value *rhs){ return BinaryInst::create_mul( lhs, rhs, this->BB_, m_);}
    BinaryInst *create_isdiv( Value *lhs, Value *rhs){ return BinaryInst::create_sdiv( lhs, rhs, this->BB_, m_);}
    
    CmpInst *create_icmp_eq( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::EQ, lhs, rhs, this->BB_, m_); }
    CmpInst *create_icmp_ne( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::NE, lhs, rhs, this->BB_, m_); }
    CmpInst *create_icmp_gt( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::GT, lhs, rhs, this->BB_, m_); }
    CmpInst *create_icmp_ge( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::GE, lhs, rhs, this->BB_, m_); }
    CmpInst *create_icmp_lt( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::LT, lhs, rhs, this->BB_, m_); }
    CmpInst *create_icmp_le( Value *lhs, Value *rhs){ return CmpInst::create_cmp(CmpInst::LE, lhs, rhs, this->BB_, m_); }

    CallInst *create_call(Value *func, std::vector<Value *> args)
    { 
        assert( dynamic_cast<Function *>(func) && "func must be Function * type"); 
        return CallInst::create(static_cast<Function *>(func) ,args, this->BB_); 
    }
    
    BranchInst *create_br(BasicBlock *if_true){ return BranchInst::create_br(if_true, this->BB_); }
    BranchInst *create_cond_br(Value *cond, BasicBlock *if_true, BasicBlock *if_false){ return BranchInst::create_cond_br(cond, if_true, if_false,this->BB_); }
    
    ReturnInst *create_ret(Value *val) { return ReturnInst::create_ret(val,this->BB_); }
    ReturnInst *create_void_ret() { return ReturnInst::create_void_ret(this->BB_); }

    GetElementPtrInst *create_gep(Value *ptr, std::vector<Value *> idxs) { return GetElementPtrInst::create_gep(ptr, idxs, this->BB_); }

    StoreInst *create_store(Value *val, Value *ptr) { return StoreInst::create_store(val, ptr, this->BB_ ); }
    LoadInst * create_load(Type *ty, Value *ptr) { return LoadInst::create_load(ty, ptr, this->BB_); }
    LoadInst * create_load(Value *ptr) 
    { 
        assert( ptr->get_type()->is_pointer_type() && "ptr must be pointer type" );
        return LoadInst::create_load(ptr->get_type()->get_pointer_element_type(), ptr, this->BB_); 
    }

    AllocaInst *create_alloca(Type *ty) { return AllocaInst::create_alloca(ty, this->BB_); }
    ZextInst *create_zext(Value *val, Type *ty) { return ZextInst::create_zext(val, ty, this->BB_); }

    SiToFpInst *create_sitofp( Value *val, Type *ty) { return SiToFpInst::create_sitofp(val, ty, this->BB_); }
    FpToSiInst *create_fptosi( Value *val, Type *ty) { return FpToSiInst::create_fptosi(val, ty, this->BB_); }
    
    FCmpInst *create_fcmp_ne( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::NE, lhs, rhs, this->BB_, m_); } 
    FCmpInst *create_fcmp_lt( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::LT, lhs, rhs, this->BB_, m_); }
    FCmpInst *create_fcmp_le( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::LE, lhs, rhs, this->BB_, m_); }
    FCmpInst *create_fcmp_ge( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::GE, lhs, rhs, this->BB_, m_); }
    FCmpInst *create_fcmp_gt( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::GT, lhs, rhs, this->BB_, m_); }
    FCmpInst *create_fcmp_eq( Value *lhs, Value *rhs) { return FCmpInst::create_fcmp(FCmpInst::EQ, lhs, rhs, this->BB_, m_); }

    BinaryInst *create_fadd( Value *lhs, Value *rhs){ return BinaryInst::create_fadd( lhs, rhs, this->BB_, m_);}
    BinaryInst *create_fsub( Value *lhs, Value *rhs){ return BinaryInst::create_fsub( lhs, rhs, this->BB_, m_);}
    BinaryInst *create_fmul( Value *lhs, Value *rhs){ return BinaryInst::create_fmul( lhs, rhs, this->BB_, m_);}
    BinaryInst *create_fdiv( Value *lhs, Value *rhs){ return BinaryInst::create_fdiv( lhs, rhs, this->BB_, m_);}
};

#endif // SYSYC_IRBUILDER_H
