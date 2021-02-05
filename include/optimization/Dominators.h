#ifndef SYSYC_DOMINATORS_HPP
#define SYSYC_DOMINATORS_HPP

#include"PassManager.hpp"
#include "BasicBlock.h"
#include <list>
#include <set>
#include <map>

class Dominators : public Pass{
public:
    explicit Dominators(Module *m) : Pass(m){}
    ~Dominators(){};
    void run() override;
    void create_doms(Function *f);
    void create_reverse_post_order(Function *f);
    void create_idom(Function *f);
    void create_dominance_frontier(Function *f);
    void create_dom_tree_succ(Function *f);

    // for debug
    void print_idom(Function *f);
    void print_dominance_frontier(Function *f);

    /****************api about Dominator****************/

    void add_dom(BasicBlock *bb, BasicBlock *dom_bb) { doms_[bb].insert(dom_bb); }
    std::set<BasicBlock *> &get_doms(BasicBlock *bb) { return doms_[bb]; }
    void set_doms(BasicBlock *bb, std::set<BasicBlock *> &doms) { doms_[bb].clear(); 
                                                doms_[bb].insert(doms.begin(), doms.end()); }
    
    BasicBlock *get_idom(BasicBlock *bb) { return idom_[bb]; }
    void set_idom(BasicBlock *bb, BasicBlock *idom) { idom_[bb] = idom; }
    
    void add_dominance_frontier(BasicBlock *bb, BasicBlock *dom_frontier_bb) { dom_frontier_[bb].insert(dom_frontier_bb); }
    std::set<BasicBlock *> &get_dominance_frontier(BasicBlock *bb) { return dom_frontier_[bb]; }
    void set_dominance_frontier(BasicBlock *bb, std::set<BasicBlock *> &df) { dom_frontier_[bb].clear(); 
                                                dom_frontier_[bb].insert(df.begin(), df.end()); }
    
        // successor blocks of this node in dominance tree
    std::set<BasicBlock *> get_dom_tree_succ_blocks(BasicBlock *bb) { return dom_tree_succ_blocks_[bb]; }
    void add_dom_tree_succ_block(BasicBlock *bb, BasicBlock *dom_tree_succ_bb) { dom_tree_succ_blocks_[bb].insert(dom_tree_succ_bb); }
    /****************api about Dominator****************/

private:
    void post_order_visit(BasicBlock *bb, std::set<BasicBlock *> &visited);
    BasicBlock *intersect(BasicBlock *b1, BasicBlock *b2);

    std::list<BasicBlock *> reverse_post_order_;
    std::map<BasicBlock *, int> post_order_id_;   // the root has highest ID

    std::map<BasicBlock *, std::set<BasicBlock *>> doms_;   // dominance set
    std::map<BasicBlock *, BasicBlock *>idom_ ;    // immediate dominance
    std::map<BasicBlock *, std::set<BasicBlock *>> dom_frontier_;    // dominance frontier set
    std::map<BasicBlock *, std::set<BasicBlock *>> dom_tree_succ_blocks_;
};

#endif