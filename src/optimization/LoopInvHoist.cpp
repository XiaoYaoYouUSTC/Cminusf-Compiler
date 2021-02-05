#include <algorithm>
#include "logging.hpp"
#include "LoopSearch.hpp"
#include "LoopInvHoist.hpp"

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