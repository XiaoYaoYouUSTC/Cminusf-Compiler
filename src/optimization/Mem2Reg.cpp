#include "Mem2Reg.hpp"
#include "IRBuilder.h"

#define IS_GLOBAL_VARIABLE(l_val) dynamic_cast<GlobalVariable *>(l_val)
#define IS_GEP_INSTR(l_val) dynamic_cast<GetElementPtrInst *>(l_val)

std::map<Value *, std::vector<Value *>> var_val_stack;//全局变量初值提前存入栈中

void Mem2Reg::run()
{
    // get info from Dominators
    dominators_ = new Dominators(m_);
    dominators_->run();
    for (auto f : m_->get_functions())
    {
        func_ = f;
        if ( func_->get_basic_blocks().size() >= 1 )
        {
            generate_phi();
            re_name(func_->get_entry_block());
        }   
        remove_alloca();
    }
}

void Mem2Reg::generate_phi()
{
    // step 1: find all global_live_var_name x and get their blocks 
    std::set<Value *> global_live_var_name;
    std::map<Value *, std::set<BasicBlock *>> live_var_2blocks;
    for ( auto bb : func_->get_basic_blocks() )
    {
        std::set<Value *> var_is_killed;
        for ( auto instr : bb->get_instructions() )
        {
            if ( instr->is_store() )
            {
                // store i32 a, i32 *b
                // a is r_val, b is l_val
                auto r_val = static_cast<StoreInst *>(instr)->get_rval();
                auto l_val = static_cast<StoreInst *>(instr)->get_lval();

                if (!IS_GLOBAL_VARIABLE(l_val) && !IS_GEP_INSTR(l_val))
                {
                    if ( var_is_killed.find(r_val) == var_is_killed.end() )
                    {
                        global_live_var_name.insert(l_val);
                    }
                    var_is_killed.insert(l_val);
                    live_var_2blocks[l_val].insert(bb);
                }
            }
        }
    }

    // step 2: insert phi instr
    std::map<std::pair<BasicBlock *,Value *>, bool> bb_has_var_phi; // bb has phi for var
    for (auto var : global_live_var_name )
    {
        std::vector<BasicBlock *> work_list;
        work_list.assign(live_var_2blocks[var].begin(), live_var_2blocks[var].end());
        for (int i =0 ; i < work_list.size() ; i++ )
        {   
            auto bb = work_list[i];
            for ( auto bb_dominance_frontier_bb : dominators_->get_dominance_frontier(bb))
            {
                if ( bb_has_var_phi.find({bb_dominance_frontier_bb, var}) == bb_has_var_phi.end() )
                { 
                    // generate phi for bb_dominance_frontier_bb & add bb_dominance_frontier_bb to work list
                    auto phi = PhiInst::create_phi(var->get_type()->get_pointer_element_type(), bb_dominance_frontier_bb);
                    phi->set_lval(var);
                    bb_dominance_frontier_bb->add_instr_begin( phi );
                    work_list.push_back( bb_dominance_frontier_bb );
                    bb_has_var_phi[{bb_dominance_frontier_bb, var}] = true;
                }
            }
        }
    }
}

void Mem2Reg::re_name(BasicBlock *bb)
{
    std::vector<Instruction *> wait_delete;

    for (auto instr : bb->get_instructions() )
    {
        if (instr->is_phi())
        {
            // step 3: push phi instr as lval's lastest value define
            auto l_val = static_cast<PhiInst *>(instr)->get_lval();
            var_val_stack[l_val].push_back(instr);         
        }
    }
    
    for (auto instr : bb->get_instructions() )
    {
        if ( instr->is_load() )
        {
            // step 4: replace load with the top of stack[l_val]
            auto l_val = static_cast<LoadInst *>(instr)->get_lval();

            if (!IS_GLOBAL_VARIABLE(l_val) && !IS_GEP_INSTR(l_val))
            {
                if ( var_val_stack.find(l_val)!=var_val_stack.end())
                {
                    instr->replace_all_use_with(var_val_stack[l_val].back());
                    wait_delete.push_back(instr);
                }
            }
        }
        if (instr->is_store())
        {
            // step 5: push r_val of store instr as lval's lastest definition
            auto l_val = static_cast<StoreInst *>(instr)->get_lval();
            auto r_val = static_cast<StoreInst *>(instr)->get_rval();

            if (!IS_GLOBAL_VARIABLE(l_val) && !IS_GEP_INSTR(l_val))
            {
                var_val_stack[l_val].push_back(r_val);
                wait_delete.push_back(instr);
            }
        } 
    }

    for ( auto succ_bb : bb->get_succ_basic_blocks() )
    {
        for ( auto instr : succ_bb->get_instructions() )
        {
            if ( instr->is_phi())
            {
                auto l_val = static_cast<PhiInst *>(instr)->get_lval();
                if (var_val_stack.find(l_val)!= var_val_stack.end())
                {                    
                    assert(var_val_stack[l_val].size()!=0);
                    // step 6: fill phi pair parameters
                    static_cast<PhiInst *>(instr)->add_phi_pair_operand( var_val_stack[l_val].back(), bb);
                }
                // else phi parameter is [ undef, bb ]
            }
        }
    }

    for ( auto dom_succ_bb : dominators_->get_dom_tree_succ_blocks(bb) )
    {
        re_name(dom_succ_bb);
    }

    for (auto instr : bb->get_instructions())
    {
        // step 7: pop lval's lastest definition
        if(instr->is_store())
        {
            auto l_val = static_cast<StoreInst *>(instr)->get_lval();
            if (!IS_GLOBAL_VARIABLE(l_val) && !IS_GEP_INSTR(l_val))
            {
                var_val_stack[l_val].pop_back();
            }
        }
        else if (instr->is_phi())
        {
            auto l_val = static_cast<PhiInst *>(instr)->get_lval();
            if ( var_val_stack.find(l_val)!=var_val_stack.end())
            {
                var_val_stack[l_val].pop_back();
            }
        }
    } 

    for ( auto instr : wait_delete)
    {
        bb->delete_instr(instr);
    }
}

void Mem2Reg::remove_alloca()
{
    for (auto bb : func_->get_basic_blocks())
    {
        std::vector<Instruction *> wait_delete;
        for (auto instr : bb->get_instructions())
        {
            auto is_alloca = dynamic_cast<AllocaInst *>(instr);
            if ( is_alloca)
            {
                bool is_int = is_alloca->get_type()->get_pointer_element_type()->is_integer_type();
                bool is_float = is_alloca->get_type()->get_pointer_element_type()->is_float_type();
                if ( is_int || is_float )
                {
                    wait_delete.push_back(instr);
                }
            }
        }
        for ( auto instr : wait_delete)
        {
            bb->delete_instr(instr);
        }
    }
}