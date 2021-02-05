#include "Dominators.h"
#include <algorithm>
#include <string>

void Dominators::run()
{
    for (auto f : m_->get_functions()) {
        if (f->get_basic_blocks().size() == 0)
            continue;
        for (auto bb : f->get_basic_blocks() )
        {
            doms_.insert({bb ,{}});
            idom_.insert({bb ,{}});
            dom_frontier_.insert({bb ,{}});
            dom_tree_succ_blocks_.insert({bb ,{}});
        }
        
        create_reverse_post_order(f);
        create_idom(f);
        create_dominance_frontier(f);
        create_dom_tree_succ(f);
        // for debug
        // print_idom(f);
        // print_dominance_frontier(f);
    }
}

void Dominators::create_doms(Function *f)
{
    // init
    for (auto bb : f->get_basic_blocks()) {
        add_dom(bb, bb);
    }
    // iterate
    bool changed = true;
    std::vector<BasicBlock *> ret(f->get_num_basic_blocks());
    std::vector<BasicBlock *> pre(f->get_num_basic_blocks());
    while (changed) {
        changed = false;
        for (auto bb : f->get_basic_blocks()) {
            auto &bbs = bb->get_pre_basic_blocks();
            auto &first = get_doms((*bbs.begin()));
            pre.insert(pre.begin(), first.begin(), first.end());
            pre.resize(first.size());
            ret.resize(f->get_num_basic_blocks());
            for (auto iter = ++bbs.begin(); iter != bbs.end(); ++iter) {
                auto &now = get_doms((*iter));
                auto it = std::set_intersection(pre.begin(), pre.end(), 
                                    now.begin(), now.end(), ret.begin());
                ret.resize(it-ret.begin());
                pre.resize(ret.size());
                pre.insert(pre.begin(), ret.begin(), ret.end());
            }
            std::set<BasicBlock *> doms;
            doms.insert(bb);
            doms.insert(pre.begin(), pre.end());
            if (get_doms(bb) != doms) {
                set_doms(bb, doms);
                changed = true;
            }
        }
    }
}

void Dominators::create_reverse_post_order(Function *f)
{
    reverse_post_order_.clear();
    post_order_id_.clear();
    std::set<BasicBlock *> visited;
    post_order_visit(f->get_entry_block(), visited);
    reverse_post_order_.reverse();
}

void Dominators::post_order_visit(BasicBlock *bb, std::set<BasicBlock *> &visited)
{
    visited.insert(bb);
    for (auto b : bb->get_succ_basic_blocks()) {
        if (visited.find(b) == visited.end())
            post_order_visit(b, visited);
    }
    post_order_id_[bb] = reverse_post_order_.size();
    reverse_post_order_.push_back(bb);
}

void Dominators::create_idom(Function *f)
{   
    // init
	for (auto bb : f->get_basic_blocks())
        set_idom(bb, nullptr);
    auto root = f->get_entry_block();
    set_idom(root, root);

    // iterate
	bool changed = true;
    while (changed) {
        changed = false;
        for (auto bb : this->reverse_post_order_) {
            if (bb == root) {
                continue;
            }

            // find one pred which has idom
            BasicBlock *pred = nullptr;
            for (auto p : bb->get_pre_basic_blocks()) {
                if (get_idom(p)) {
                    pred = p;
                    break;
                }
            }
            assert(pred);

            BasicBlock *new_idom = pred;
            for (auto p : bb->get_pre_basic_blocks()) {
                if (p == pred)
                    continue;
                if (get_idom(p)) {
                    new_idom = intersect(p, new_idom);
                }
            }
            if (get_idom(bb) != new_idom) {
                set_idom(bb, new_idom);
                changed = true;
            }
        }
    }
    
}

// find closest parent of b1 and b2
BasicBlock *Dominators::intersect(BasicBlock *b1, BasicBlock *b2)
{
    while (b1 != b2) {
        while (post_order_id_[b1] < post_order_id_[b2]) {
            assert(get_idom(b1));
            b1 = get_idom(b1);
        }
        while (post_order_id_[b2] < post_order_id_[b1]) {
            assert(get_idom(b2));
            b2 = get_idom(b2);
        }
    }
    return b1;   
}

void Dominators::create_dominance_frontier(Function *f)
{
    for (auto bb : f->get_basic_blocks()) {
        if (bb->get_pre_basic_blocks().size() >= 2) {
            for (auto p : bb->get_pre_basic_blocks()) {
                auto runner = p;
                while (runner != get_idom(bb)) {
                    add_dominance_frontier(runner, bb);
                    runner = get_idom(runner);
                }
            }
        }
    }
}

void Dominators::create_dom_tree_succ(Function *f)
{
    for (auto bb : f->get_basic_blocks()) {
        auto idom = get_idom(bb);
        // e.g, entry bb
        if (idom != bb) {
            add_dom_tree_succ_block(idom, bb);
        }
    }
}

void Dominators::print_idom(Function *f)
{
    int counter = 0;
    std::map<BasicBlock *, std::string> bb_id;
    for (auto bb : f->get_basic_blocks()) {
        if (bb->get_name().empty())
            bb_id[bb] = "bb" + std::to_string(counter);
        else 
            bb_id[bb] = bb->get_name();
        counter++;
    }
    printf("Immediate dominance of function %s:\n", f->get_name().c_str());
    for (auto bb : f->get_basic_blocks()) {
        std::string output;
        output = bb_id[bb] + ": ";
        if (get_idom(bb)) {
            output += bb_id[get_idom(bb)];
        }
        else {
            output += "null";
        }
        printf("%s\n", output.c_str());
    }
}

void Dominators::print_dominance_frontier(Function *f)
{
    int counter = 0;
    std::map<BasicBlock *, std::string> bb_id;
    for (auto bb : f->get_basic_blocks()) {
        if (bb->get_name().empty())
            bb_id[bb] = "bb" + std::to_string(counter);
        else 
            bb_id[bb] = bb->get_name();
        counter++;
    }
    printf("Dominance Frontier of function %s:\n", f->get_name().c_str());
    for (auto bb : f->get_basic_blocks()) {
        std::string output;
        output = bb_id[bb] + ": ";
        if (get_dominance_frontier(bb).empty()) {
            output += "null";
        }
        else {
            bool first = true;
            for (auto df : get_dominance_frontier(bb)) {
                if (first) {
                    first = false;
                }
                else {
                    output += ", ";
                }
                output += bb_id[df];
            }
        }
        printf("%s\n", output.c_str());
    }
}

