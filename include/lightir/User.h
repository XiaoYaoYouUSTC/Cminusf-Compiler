#ifndef SYSYC_USER_H
#define SYSYC_USER_H

#include "Value.h"
#include <vector>
// #include <memory>

class User : public Value
{
public:
    User(Type *ty, const std::string &name = "", unsigned num_ops = 0);
    ~User() = default;

    std::vector<Value *>& get_operands();

    // start from 0
    Value *get_operand(unsigned i) const;

    // start from 0
    void set_operand(unsigned i, Value *v);
    void add_operand( Value *v);

    unsigned get_num_operand() const;

    void remove_use_of_ops();
    void remove_operands(int index1,int index2);

private:
    // std::unique_ptr< std::list<Value *> > operands_;   // operands of this value
    std::vector<Value *> operands_;   // operands of this value
    unsigned num_ops_;
};

#endif // SYSYC_USER_H
