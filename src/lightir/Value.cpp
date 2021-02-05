#include "Value.h"
#include "Type.h"
#include "User.h"
#include <cassert>

Value::Value(Type *ty, const std::string &name )
  : type_(ty), name_(name)
{

}

void Value::add_use(Value *val, unsigned arg_no )
{
    use_list_.push_back(Use(val, arg_no));
}

std::string Value::get_name() const
{
    return name_;
}

void Value::replace_all_use_with(Value *new_val)
{
    for (auto use : use_list_) {
        auto val = dynamic_cast<User *>(use.val_);
        assert(val && "new_val is not a user");
        val->set_operand(use.arg_no_, new_val);
    }
}

void Value::remove_use(Value *val)
{
    auto is_val = [val] (const Use &use) { return use.val_ == val; };
    use_list_.remove_if(is_val);
}