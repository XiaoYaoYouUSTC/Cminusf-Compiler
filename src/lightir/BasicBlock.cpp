#include "Module.h"
#include "BasicBlock.h"
#include "Function.h"
#include "IRprinter.h"
#include <cassert>

BasicBlock::BasicBlock(Module *m, const std::string &name = "",
                      Function *parent = nullptr)
    : Value(Type::get_label_type(m), name), parent_(parent)
{
    assert(parent && "currently parent should not be nullptr");
    parent_->add_basic_block(this);
}

Module *BasicBlock::get_module()
{
    return get_parent()->get_parent();
}

void BasicBlock::add_instruction(Instruction *instr)
{
    instr_list_.push_back(instr);
}

void BasicBlock::add_instr_begin(Instruction *instr)
{
    instr_list_.push_front(instr);
}

void BasicBlock::delete_instr( Instruction *instr )
{
    instr_list_.remove(instr);
    instr->remove_use_of_ops();
}

const Instruction *BasicBlock::get_terminator() const
{
    if (instr_list_.empty()){
        return nullptr;
    }
    switch (instr_list_.back()->get_instr_type())
    {
    case Instruction::ret:
        return instr_list_.back();
        break;
    
    case Instruction::br:
        return instr_list_.back();
        break;

    default:
        return nullptr;
        break;
    }
}

void BasicBlock::erase_from_parent()
{
    this->get_parent()->remove(this);
}

std::string BasicBlock::print()
{
    std::string bb_ir;
    bb_ir += this->get_name();
    bb_ir += ":";
    // print prebb
    if(!this->get_pre_basic_blocks().empty())
    {
        bb_ir += "                                                ; preds = ";
    }
    for (auto bb : this->get_pre_basic_blocks() )
    {
        if( bb != *this->get_pre_basic_blocks().begin() )
            bb_ir += ", ";
        bb_ir += print_as_op(bb, false);
    }
    
    // print prebb
    if ( !this->get_parent() )
    {
        bb_ir += "\n";
        bb_ir += "; Error: Block without parent!";
    }
    bb_ir += "\n";
    for ( auto instr : this->get_instructions() )
    {
        bb_ir += "  ";
        bb_ir += instr->print();
        bb_ir += "\n";
    }

    return bb_ir;
}