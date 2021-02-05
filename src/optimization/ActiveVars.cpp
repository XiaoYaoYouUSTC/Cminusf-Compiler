#include "ActiveVars.hpp"

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

std::string ActiveVars::print()
{
    std::string active_vars;
    active_vars +=  "{\n";
    active_vars +=  "\"function\": \"";
    active_vars +=  func_->get_name();
    active_vars +=  "\",\n";

    active_vars +=  "\"live_in\": {\n";
    for (auto &p : live_in) {
        if (p.second.size() == 0) {
            continue;
        } else {
            active_vars +=  "  \"";
            active_vars +=  p.first->get_name();
            active_vars +=  "\": [" ;
            for (auto &v : p.second) {
                active_vars +=  "\"%";
                active_vars +=  v->get_name();
                active_vars +=  "\",";
            }
            active_vars += "]" ;
            active_vars += ",\n";   
        }
    }
    active_vars += "\n";
    active_vars +=  "    },\n";
    
    active_vars +=  "\"live_out\": {\n";
    for (auto &p : live_out) {
        if (p.second.size() == 0) {
            continue;
        } else {
            active_vars +=  "  \"";
            active_vars +=  p.first->get_name();
            active_vars +=  "\": [" ;
            for (auto &v : p.second) {
                active_vars +=  "\"%";
                active_vars +=  v->get_name();
                active_vars +=  "\",";
            }
            active_vars += "]";
            active_vars += ",\n";
        }
    }
    active_vars += "\n";
    active_vars += "    }\n";

    active_vars += "}\n";
    active_vars += "\n";
    return active_vars;
}