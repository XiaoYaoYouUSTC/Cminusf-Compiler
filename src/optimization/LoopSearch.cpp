#include "LoopSearch.hpp"
#include <iostream>
#include <unordered_set>
#include <fstream>
#include "logging.hpp"

struct CFGNode
{
    std::unordered_set<CFGNodePtr> succs;
    std::unordered_set<CFGNodePtr> prevs;
    BasicBlock *bb;
    int index;   // the index of the node in CFG
    int lowlink; // the min index of the node in the strongly connected componets
    bool onStack;
};

// build control flow graph used in loop search pass
void LoopSearch::build_cfg(Function *func, std::unordered_set<CFGNode *> &result)
{
    std::unordered_map<BasicBlock *, CFGNode *> bb2cfg_node;
    for (auto bb : func->get_basic_blocks())
    {
        auto node = new CFGNode;
        node->bb = bb;
        node->index = node->lowlink = -1;
        node->onStack = false;
        bb2cfg_node.insert({bb, node});

        result.insert(node);
    }
    for (auto bb : func->get_basic_blocks())
    {
        auto node = bb2cfg_node[bb];
        std::string succ_string = "success node: ";
        for (auto succ : bb->get_succ_basic_blocks())
        {
            succ_string = succ_string + succ->get_name() + " ";
            node->succs.insert(bb2cfg_node[succ]);
        }
        std::string prev_string = "previous node: ";
        for (auto prev : bb->get_pre_basic_blocks())
        {
            prev_string = prev_string + prev->get_name() + " ";
            node->prevs.insert(bb2cfg_node[prev]);
        }
    }
}

// Tarjan algorithm
// reference: https://baike.baidu.com/item/tarjan%E7%AE%97%E6%B3%95/10687825?fr=aladdin
bool LoopSearch::strongly_connected_components(
    CFGNodePtrSet &nodes,
    std::unordered_set<CFGNodePtrSet *> &result)
{
    index_count = 0;
    stack.clear();
    for (auto n : nodes)
    {
        if (n->index == -1)
            traverse(n, result);
    }
    return result.size() != 0;
}
void LoopSearch::traverse(
    CFGNodePtr n,
    std::unordered_set<CFGNodePtrSet *> &result)
{
    n->index = index_count++;
    n->lowlink = n->index;
    stack.push_back(n);
    n->onStack = true;

    for (auto su : n->succs)
    {
        // has not visited su
        if (su->index == -1)
        {
            traverse(su, result);
            n->lowlink = std::min(su->lowlink, n->lowlink);
        }
        // has visited su
        else if (su->onStack)
        {
            n->lowlink = std::min(su->index, n->lowlink);
        }
    }

    // nodes that in the same strongly connected component will be popped out of stack
    if (n->index == n->lowlink)
    {
        auto set = new CFGNodePtrSet;
        CFGNodePtr tmp;
        do
        {
            tmp = stack.back();
            tmp->onStack = false;
            set->insert(tmp);
            stack.pop_back();
        } while (tmp != n);
        if (set->size() == 1)
            delete set;
        else
            result.insert(set);
    }
}
CFGNodePtr LoopSearch::find_loop_base(
    CFGNodePtrSet *set,
    CFGNodePtrSet &reserved)
{

    CFGNodePtr base = nullptr;
    bool hadBeen = false;
    for (auto n : *set)
    {
        for (auto prev : n->prevs)
        {
            if (set->find(prev) == set->end())
            {
                base = n;
            }
        }
    }
    if (base != nullptr)
        return base;
    for (auto res : reserved)
    {
        for (auto succ : res->succs)
        {
            if (set->find(succ) != set->end())
            {
                base = succ;
            }
        }
    }

    return base;
}
void LoopSearch::run()
{
    

    auto func_list = m_->get_functions();
    for (auto func : func_list)
    {

        if (func->get_basic_blocks().size() == 0)
        {
            continue;
        }
        else
        {
            CFGNodePtrSet nodes;
            CFGNodePtrSet reserved;
            std::unordered_set<CFGNodePtrSet *> sccs;
            
            // step 1: build cfg
            build_cfg(func, nodes);
            // dump graph
            dump_graph(nodes, func->get_name());
            // step 2: find strongly connected graph from external to internal
            int scc_index = 0;
            while (strongly_connected_components(nodes, sccs))
            {

                if (sccs.size() == 0)
                {
                    
                    break;
                }
                else
                {
                    // step 3: find loop base node for each strongly connected graph

                    for (auto scc : sccs)
                    {
                        scc_index += 1;

                        auto base = find_loop_base(scc, reserved);

                        // step 4: store result
                        auto bb_set = new BBset_t;
                        std::string node_set_string = "";

                        for (auto n : *scc)
                        {
                            bb_set->insert(n->bb);
                            node_set_string = node_set_string + n->bb->get_name() + ',';
                        }
                        loop_set.insert(bb_set);
                        func2loop[func].insert(bb_set);
                        base2loop.insert({base->bb, bb_set});
                        loop2base.insert({bb_set, base->bb});
                        for (auto bb : *bb_set)
                        {
                            if (bb2base.find(bb) == bb2base.end())
                            {
                                bb2base.insert({bb, base->bb});
                            }
                            else
                            {
                                bb2base[bb] = base->bb;
                            }
                        }
                        // step 5: map each node to loop base
                        for (auto bb : *bb_set)
                        {
                            if (bb2base.find(bb) == bb2base.end())
                                bb2base.insert({bb, base->bb});
                            else
                                bb2base[bb] = base->bb;
                        }

                        // step 6: remove loop base node for researching inner loop
                        reserved.insert(base);
                        dump_graph(*scc, func->get_name() + '_' + std::to_string(scc_index));
                        nodes.erase(base);
                        for (auto su : base->succs)
                        {
                            su->prevs.erase(base);
                        }
                        for (auto prev : base->prevs)
                        {
                            prev->succs.erase(base);
                        }

                    } // for (auto scc : sccs)
                    for (auto scc : sccs)
                        delete scc;
                    sccs.clear();
                    for (auto n : nodes)
                    {
                        n->index = n->lowlink = -1;
                        n->onStack = false;
                    }
                } // else
            }     // while (strongly_connected_components(nodes, sccs))
            // clear
            reserved.clear();
            for (auto node : nodes)
            {
                delete node;
            }
            nodes.clear();
        } // else

    } // for (auto func : func_list)
}

void LoopSearch::dump_graph(CFGNodePtrSet &nodes, std::string title)
{
    if (dump)
    {
        std::vector<std::string> edge_set;
        for (auto node : nodes)
        {
            if (node->bb->get_name() == "")
            {
                
                return;
            }
            if (base2loop.find(node->bb) != base2loop.end())
            {

                for (auto succ : node->succs)
                {
                    if (nodes.find(succ) != nodes.end())
                    {
                        edge_set.insert(edge_set.begin(), '\t' + node->bb->get_name() + "->" + succ->bb->get_name() + ';' + '\n');
                    }
                }
                edge_set.insert(edge_set.begin(), '\t' + node->bb->get_name() + " [color=red]" + ';' + '\n');
            }
            else
            {
                for (auto succ : node->succs)
                {
                    if (nodes.find(succ) != nodes.end())
                    {
                        edge_set.push_back('\t' + node->bb->get_name() + "->" + succ->bb->get_name() + ';' + '\n');
                    }
                }
            }
        }
        std::string digragh = "digraph G {\n";
        for (auto edge : edge_set)
        {
            digragh += edge;
        }
        digragh += '}';
        std::ofstream file_output;
        file_output.open(title + ".dot", std::ios::out);
        
        file_output << digragh;
        file_output.close();
        std::string dot_cmd = "dot -Tpng " + title + ".dot" + " -o " + title + ".png";
        std::system(dot_cmd.c_str());
    }
}

BBset_t *LoopSearch::get_parent_loop(BBset_t *loop)
{
    auto base = loop2base[loop];
    for (auto prev : base->get_pre_basic_blocks())
    {
        if (loop->find(prev) != loop->end())
            continue;
        auto loop = get_inner_loop(prev);
        if (loop == nullptr || loop->find(base) == loop->end())
            return nullptr;
        else
        {
            return loop;
        }
    }
    return nullptr;
}

std::unordered_set<BBset_t *> LoopSearch::get_loops_in_func(Function *f)
{
    return func2loop.count(f) ? func2loop[f] : std::unordered_set<BBset_t *>();
}
