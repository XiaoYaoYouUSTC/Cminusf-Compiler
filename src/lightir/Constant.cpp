#include "Constant.h"
#include "Module.h"
#include <iostream>
#include <sstream>

ConstantInt *ConstantInt::get(int val, Module *m)
{
    return new ConstantInt(Type::get_int32_type(m), val);
}
ConstantInt *ConstantInt::get(bool val, Module *m)
{
    return new ConstantInt(Type::get_int1_type(m),val?1:0);
}
std::string ConstantInt::print()
{
    std::string const_ir;
    Type *ty = this->get_type();
    if ( ty->is_integer_type() && static_cast<IntegerType *>(ty)->get_num_bits() == 1 )
    {
        //int1
        const_ir += (this->get_value() == 0) ? "false" : "true";
    }
    else
    {
        //int32
        const_ir += std::to_string(this->get_value());
    }
    return const_ir;
}

ConstantArray::ConstantArray(ArrayType *ty, const std::vector<Constant*> &val)
    : Constant(ty, "", val.size()) 
{
    for (int i = 0; i < val.size(); i++)
        set_operand(i, val[i]);
    this->const_array.assign(val.begin(),val.end());
}

Constant *ConstantArray::get_element_value(int index) {
    return this->const_array[index];
}

ConstantArray *ConstantArray::get(ArrayType *ty, const std::vector<Constant*> &val)
{
    return new ConstantArray(ty, val);
}

std::string ConstantArray::print()
{
    std::string const_ir;
    const_ir += this->get_type()->print();
    const_ir += " ";
    const_ir += "[";
    for ( int i = 0 ; i < this->get_size_of_array() ; i++ )
    {
        const_ir += get_element_value(i)->print();
        const_ir += ", ";
    }
    return const_ir;
}

ConstantFP *ConstantFP::get(float val, Module *m) 
{
    return new ConstantFP(Type::get_float_type(m), val);
}

std::string ConstantFP::print() 
{
    std::stringstream fp_ir_ss;
    std::string fp_ir;
    double val = this->get_value();
    fp_ir_ss << "0x"<< std::hex << *(uint64_t *)&val << std::endl;
    fp_ir_ss >> fp_ir; 
    return fp_ir;
}

ConstantZero *ConstantZero::get(Type *ty, Module *m) 
{
    return new ConstantZero(ty);
}

std::string ConstantZero::print()
{
    return "zeroinitializer";
}
