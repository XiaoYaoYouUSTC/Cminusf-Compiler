#include "Type.h"
#include "Module.h"

#include <cassert>

Type::Type(TypeID tid, Module *m)
{
    tid_ = tid;
    m_ = m;
}

Module *Type::get_module()
{
    return m_;
}

bool Type::is_eq_type(Type *ty1, Type *ty2)
{
    return ty1 == ty2;
}

Type *Type::get_void_type(Module *m)
{
    return m->get_void_type();
}

Type *Type::get_label_type(Module *m)
{
    return m->get_label_type();
}

IntegerType *Type::get_int1_type(Module *m)
{
    return m->get_int1_type();
}

IntegerType *Type::get_int32_type(Module *m)
{
    return m->get_int32_type();
}

PointerType *Type::get_pointer_type(Type *contained)
{
    return PointerType::get(contained);
}

ArrayType *Type::get_array_type(Type *contained, unsigned num_elements)
{
    return ArrayType::get(contained, num_elements);
}

PointerType *Type::get_int32_ptr_type(Module *m)
{
    return m->get_int32_ptr_type();
}

FloatType *Type::get_float_type(Module *m) 
{
    return m->get_float_type();
}

PointerType *Type::get_float_ptr_type(Module *m) 
{
    return m->get_float_ptr_type();
}

Type *Type::get_pointer_element_type(){ 
    if( this->is_pointer_type() )
        return static_cast<PointerType *>(this)->get_element_type();
    else
        return nullptr;
}

Type *Type::get_array_element_type(){
    if( this->is_array_type() )
        return static_cast<ArrayType *>(this)->get_element_type();
    else
        return nullptr;
}

int Type::get_size() 
{
    if (this->is_integer_type()) 
    {
        auto bits = static_cast<IntegerType *>(this)->get_num_bits() / 8;
        return bits > 0 ? bits : 1;        
    }
    if (this->is_array_type()) 
    {
        auto element_size = static_cast<ArrayType *>(this)->get_element_type()->get_size();
        auto num_elements = static_cast<ArrayType *>(this)->get_num_of_elements();
        return element_size * num_elements;
    }
    if (this->is_pointer_type()) 
    {
        if (this->get_pointer_element_type()->is_array_type()) 
        {
            return this->get_pointer_element_type()->get_size();
        } 
        else 
        {
            return 4;
        }
    }
    if(this->is_float_type())
    {
        return 4;
    }
    return 0;
}

std::string Type::print(){
    std::string type_ir;
    switch (this->get_type_id())
    {
    case VoidTyID:
        type_ir += "void";
        break;
    case LabelTyID:
        type_ir += "label";
        break;
    case IntegerTyID:
        type_ir += "i";
        type_ir += std::to_string( static_cast<IntegerType *>(this)->get_num_bits());
        break;
    case FunctionTyID:
        type_ir += static_cast<FunctionType *>(this)->get_return_type()->print();
        type_ir += " (";
        for( int i = 0 ; i < static_cast<FunctionType *>(this)->get_num_of_args() ; i++)
        {
            if(i)
                type_ir += ", ";
            type_ir += static_cast<FunctionType *>(this)->get_param_type(i)->print();
        }
        type_ir += ")";
        break;
    case PointerTyID:
        type_ir += this->get_pointer_element_type()->print();
        type_ir += "*";
        break;
    case ArrayTyID:
        type_ir += "[";
        type_ir += std::to_string( static_cast<ArrayType *>(this)->get_num_of_elements());
        type_ir += " x ";
        type_ir += static_cast<ArrayType *>(this)->get_element_type()->print();
        type_ir += "]";
        break;
    case FloatTyID:
        type_ir += "float";
        break;
    default:
        break;
    }
    return type_ir;
}

IntegerType::IntegerType(unsigned num_bits , Module *m)
    : Type(Type::IntegerTyID, m), num_bits_(num_bits)
{
}

IntegerType *IntegerType::get(unsigned num_bits, Module *m )
{
    return new IntegerType(num_bits, m);
}

unsigned IntegerType::get_num_bits()
{
    return num_bits_;
}

FunctionType::FunctionType(Type *result, std::vector<Type *> params)
    : Type(Type::FunctionTyID, nullptr)
{
    assert(is_valid_return_type(result) && "Invalid return type for function!");
    result_ = result;

    for (auto p : params) {
        assert(is_valid_argument_type(p) &&
            "Not a valid type for function argument!");
        args_.push_back(p);
    }
}

bool FunctionType::is_valid_return_type(Type *ty)
{
    return ty->is_integer_type() || ty->is_void_type() || ty->is_float_type();
}

bool FunctionType::is_valid_argument_type(Type *ty)
{
    return ty->is_integer_type() || ty->is_pointer_type() || ty->is_float_type();
}

FunctionType *FunctionType::get(Type *result,
                            std::vector<Type*> params)
{
    return new FunctionType(result, params);
}

unsigned FunctionType::get_num_of_args() const
{
    return args_.size();
}

Type *FunctionType::get_param_type(unsigned i) const
{
    return args_[i];
}

Type *FunctionType::get_return_type() const
{
    return result_;
}

ArrayType::ArrayType(Type *contained, unsigned num_elements)
    : Type(Type::ArrayTyID, contained->get_module()), num_elements_(num_elements)
{
    assert(is_valid_element_type(contained) && "Not a valid type for array element!");
    contained_ = contained;
}

bool ArrayType::is_valid_element_type(Type *ty)
{
    return ty->is_integer_type()||ty->is_array_type()||ty->is_float_type();
}

ArrayType *ArrayType::get(Type *contained, unsigned num_elements)
{
    return contained->get_module()->get_array_type(contained, num_elements);
}

PointerType::PointerType(Type *contained)
    : Type(Type::PointerTyID, contained->get_module()), contained_(contained)
{
    
}

PointerType *PointerType::get(Type *contained)
{
    return contained->get_module()->get_pointer_type(contained);
}

FloatType::FloatType (Module *m) 
    : Type(Type::FloatTyID, m) 
{

}
