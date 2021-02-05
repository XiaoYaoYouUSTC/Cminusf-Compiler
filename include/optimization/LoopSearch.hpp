#ifndef LOOPSEARCH_HPP
#define LOOPSEARCH_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "PassManager.hpp"
#include "Function.h"
#include "BasicBlock.h"
#include <memory>
#include <vector>
struct CFGNode;
using CFGNodePtr = CFGNode*;
using CFGNodePtrSet = std::unordered_set<CFGNode*>;
using BBset_t = std::unordered_set<BasicBlock *>;



class LoopSearch : public Pass{
public:
    explicit LoopSearch(Module* m, bool dump=false) : Pass(m), dump(dump){}
    ~LoopSearch() = default;
    void build_cfg(Function *func,std::unordered_set<CFGNode *> &result);
    void run() override;
    bool strongly_connected_components(
        CFGNodePtrSet &nodes,
        std::unordered_set<CFGNodePtrSet *> &result
    );
    void dump_graph(CFGNodePtrSet& nodes,std::string title);
    void traverse(
        CFGNodePtr n, 
        std::unordered_set<CFGNodePtrSet *> &result);
    CFGNodePtr find_loop_base(
        CFGNodePtrSet *set,
        CFGNodePtrSet &reserved);

    // 利用iterator来遍历所有的loop
    auto begin(){ return loop_set.begin();}
    auto end(){ return loop_set.end(); }

    BasicBlock* get_loop_base(BBset_t *loop) { return loop2base[loop]; }
    
    // 得到bb所在最低层次的loop 
    BBset_t *get_inner_loop(BasicBlock* bb){
        if(bb2base.find(bb) == bb2base.end())
            return nullptr;
        return base2loop[bb2base[bb]];
    }

    // 得到输入loop的外一层的循环，如果没有则返回空
    BBset_t *get_parent_loop(BBset_t *loop);
    
    // 得到函数 f 里的所有循环
    std::unordered_set<BBset_t *> get_loops_in_func(Function *f);
    

    
private:
    int index_count;
    bool dump;
    std::vector<CFGNodePtr> stack;
    // loops found
    std::unordered_set<BBset_t *> loop_set;
    // loops found in a function
    std::unordered_map<Function *, std::unordered_set<BBset_t *>> func2loop;
    // { entry bb of loop : loop }
    std::unordered_map<BasicBlock *, BBset_t *> base2loop;
    // { loop : entry bb of loop }
    std::unordered_map<BBset_t *, BasicBlock *> loop2base;
    // { bb :  entry bb of loop} 默认最低层次的loop
    std::unordered_map<BasicBlock *, BasicBlock *> bb2base;
};




#endif
