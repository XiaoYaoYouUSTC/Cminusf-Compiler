#ifndef SYSYC_BASICBLOCK_H
#define SYSYC_BASICBLOCK_H

#include "Value.h"
#include "Instruction.h"
#include "Module.h"
#include "Function.h"

#include <list>
#include <set>
#include <string>

class Function;
class Instruction;
class Module;

class BasicBlock : public Value
{
public:
    static BasicBlock *create(Module *m, const std::string &name ,
                            Function *parent ) {
        auto prefix = name.empty() ? "" : "label_";
        return new BasicBlock(m, prefix + name, parent);
    }

    // return parent, or null if none.
    Function *get_parent() { return parent_; }
    
    Module *get_module();

    /****************api about cfg****************/

    std::list<BasicBlock *> &get_pre_basic_blocks() { return pre_bbs_; }
    std::list<BasicBlock *> &get_succ_basic_blocks() { return succ_bbs_; }
    void add_pre_basic_block(BasicBlock *bb) { pre_bbs_.push_back(bb); }
    void add_succ_basic_block(BasicBlock *bb) { succ_bbs_.push_back(bb); }

    void remove_pre_basic_block(BasicBlock *bb) { pre_bbs_.remove(bb); }
    void remove_succ_basic_block(BasicBlock *bb) { succ_bbs_.remove(bb); }

    /****************api about cfg****************/

    /// Returns the terminator instruction if the block is well formed or null
    /// if the block is not well formed.
    const Instruction *get_terminator() const;
    Instruction *get_terminator() {
        return const_cast<Instruction *>(
            static_cast<const BasicBlock *>(this)->get_terminator());
    }
    
    void add_instruction(Instruction *instr);
    void add_instr_begin(Instruction *instr);

    void delete_instr(Instruction *instr);

    bool empty() { return instr_list_.empty(); }

    int get_num_of_instr() { return instr_list_.size(); }
    std::list<Instruction *> &get_instructions() { return instr_list_; }
    
    void erase_from_parent();
    
    virtual std::string print() override;

private:
    explicit BasicBlock(Module *m, const std::string &name ,
                        Function *parent );
    std::list<BasicBlock *> pre_bbs_;
    std::list<BasicBlock *> succ_bbs_;
    std::list<Instruction *> instr_list_;
    Function *parent_;

};

#endif // SYSYC_BASICBLOCK_H