#include "cminusf_builder.hpp"

// use these macros to get constant value
#define CONST_INT(num) \
    ConstantInt::get(num, module.get())

#define CONST_FP(num) \
    ConstantFP::get((float)num, module.get())

#define CONST_ZERO(type) \
    ConstantZero::get(type, module.get())


// You can define global variables here
// to store state
Value* expression;
//用来记录assign_expression和simple_expression的值
//更新：全部的表达式值均使用expression存储
Value* var_location;
//这个只有在assignexpression和call中才动用，你就当没看见

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

void CminusfBuilder::visit(ASTProgram &node) {
    for(auto decl:node.declarations)
    {
        decl->accept(*this);
    }
    //访问每一个Declaration结点
 }

void CminusfBuilder::visit(ASTNum &node) { 
    if(node.type==TYPE_INT)//整型
        expression = ConstantInt::get(node.i_val,module.get());
    else if (node.type==TYPE_FLOAT)//浮点型
        expression= ConstantFP::get(node.f_val, module.get());
}

void CminusfBuilder::visit(ASTVarDeclaration &node) {
    Type* var_type;
    if(node.type==TYPE_INT)
        var_type=Type::get_int32_type(module.get());
    else if (node.type==TYPE_FLOAT)
        var_type=Type::get_float_type(module.get());
    auto initializer=CONST_ZERO(var_type);
    if(scope.in_global())
    {//针对全局变量处理，全局变量要赋初值申请方式也不同
    //in_global判断是否在全局scope中
        if(node.num==nullptr)
        { //通过判断node.num是否存在来确定是是否为数组
            auto global_var=GlobalVariable::create(node.id,module.get(),var_type,false,initializer);
            scope.push(node.id,global_var);
        }
        else
        {
            auto *arrayType=ArrayType::get(var_type,node.num->i_val);
            auto global_var_array=GlobalVariable::create(node.id,module.get(),arrayType,false,initializer);
            scope.push(node.id,global_var_array);
        } 
    }
    else
    {//同理，针对local变量
        if(node.num==nullptr)
        {
            auto varAlloca = builder->create_alloca(var_type);
            scope.push(node.id,varAlloca);

        }
        else
        {
            auto *arrayType=ArrayType::get(var_type,node.num->i_val);
            auto varArrayAlloca=builder->create_alloca(arrayType);
            scope.push(node.id,varArrayAlloca);
            //printf("array");
        }
        
    }
    
 }
 
void CminusfBuilder::visit(ASTFunDeclaration &node) { 
    std::vector<Type *> Vars;
    for (auto param: node.params) {//参数列表类型
        if (param->isarray) {
            if(param->type==TYPE_INT)
                Vars.push_back(Type::get_int32_ptr_type(module.get()));
            else if(param->type==TYPE_FLOAT)
                Vars.push_back(Type::get_float_ptr_type(module.get()));
        }
        else {
            if(param->type==TYPE_INT)
                Vars.push_back(Type::get_int32_type(module.get()));
            else if(param->type==TYPE_FLOAT)
                Vars.push_back(Type::get_float_type(module.get()));
        }
    }
    Type* functype;
    if(node.type==TYPE_INT)//函数类型
        functype = Type::get_int32_type(module.get());
    else if(node.type==TYPE_FLOAT)
        functype = Type::get_float_type(module.get());
    else 
        functype = Type::get_void_type(module.get());
   
    auto function = Function::create(FunctionType::get(functype, Vars), 
            node.id, 
            module.get());
    scope.push(node.id,function);
    //全局作用域内存放函数
    scope.enter();
    //进入函数的作用域
    auto entrybb = BasicBlock::create(module.get(), "", function);
    builder->set_insert_point(entrybb);
    // 为参数分配空间
    auto arg = function->arg_begin();
    Value* param_var;
    Value* param_value;
    // = builder->create_alloca(Type::get_int32_ptr_type(module.get()));
    std::vector<Value *> args1;
    int i=0;
    for (auto param: node.params) {
        args1.push_back(*arg);
        if (param->isarray) {//当参数为指针时
            if(param->type==TYPE_INT)
                 param_var = builder->create_alloca(Type::get_int32_ptr_type(module.get()));
            else if(param->type==TYPE_FLOAT)
                 param_var = builder->create_alloca(Type::get_float_ptr_type(module.get()));
            builder->create_store(args1[i], param_var);
            param_value=builder->create_load(param_var);
            scope.push(param->id, param_value);
        }
        else {//当参数为i32 or float
            if(param->type==TYPE_INT)
                 param_var = builder->create_alloca(Type::get_int32_type(module.get()));
            else if(param->type==TYPE_FLOAT)
                 param_var = builder->create_alloca(Type::get_float_type(module.get()));
            builder->create_store(args1[i],param_var);
            scope.push(param->id, param_var);
        }
        arg++;
        i++;
    }
    node.compound_stmt->accept(*this);  
    //访问compound
    scope.exit();
    //退出作用域
}

void CminusfBuilder::visit(ASTParam &node) {
//在fundeclaration中已处理Param
}

void CminusfBuilder::visit(ASTCompoundStmt &node) { 
    scope.enter();
    //进入新的作用域
    for(auto locdecl:node.local_declarations)
    {
        locdecl->accept(*this);
    }
    for(auto stmtl:node.statement_list)
    {
        stmtl->accept(*this);
        if(builder->get_insert_block()->get_terminator()!=nullptr)
            break;
    }
    scope.exit();
    //退出作用域
}

void CminusfBuilder::visit(ASTExpressionStmt &node) {
    if(node.expression!=nullptr)
        node.expression->accept(*this);
 }

void CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto fun=builder->get_insert_block()->get_parent();
    //获得当前所在的函数
    //不加get_parent就是获得当前缩在bb，超好用
    node.expression->accept(*this);
    auto type_exp=expression->get_type();
    Value* true_or_false;
    if(type_exp->is_integer_type()==1)
    {//不知道如何处理bool形
        auto type_zero=Type::get_int32_type(module.get());
        true_or_false=builder->create_icmp_gt(expression,CONST_ZERO(type_zero));
    }
    else 
    {
        auto type_zero=Type::get_float_type(module.get());
        true_or_false=builder->create_fcmp_gt(expression,CONST_ZERO(type_zero));
    }
    //由于expression只有int和float两种，故通过区分并与0比较确定为真or假
    if(node.else_statement!=nullptr)
    {
        auto trueBB=BasicBlock::create(module.get(),"",fun);
        auto falseBB=BasicBlock::create(module.get(),"",fun);
        //auto mergeBB=BasicBlock::create(module.get(),"mergeBB",fun);
        BasicBlock* mergeBB;
        //分别为真时的分支、为假的分支；
        builder->create_cond_br(true_or_false,trueBB,falseBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);
        auto trueterm=builder->get_insert_block()->get_terminator();
        auto curtrueBB=builder->get_insert_block();
        //需要判断是否有ret
        //经过遍历trueBB的结点后，BB可能已经变换，需要存储当前BB。

        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        auto falseterm=builder->get_insert_block()->get_terminator();
        auto curfalseBB=builder->get_insert_block();
        //对于falseBB同上

        if(trueterm==nullptr||falseterm==nullptr)
            mergeBB=BasicBlock::create(module.get(),"",fun);
        //只有当trueBB或falseBB未出现ret语句时，才会有br的需要，才需要有mergeBB
        
        if(trueterm==nullptr)
        {
            builder->set_insert_point(curtrueBB);  
            builder->create_br(mergeBB);
        }
        //当true分支没有出现ret语句，才需要br

        if(falseterm==nullptr)
        {
            builder->set_insert_point(curfalseBB);
            builder->create_br(mergeBB);
        }
        //同理

        if(trueterm==nullptr||falseterm==nullptr)
            builder->set_insert_point(mergeBB);
        //如果二者均有了ret,则当前的if语句也不存在mergeBB
        //相当于一条断头路,不需要设置到mergeBB
        //两个分支都return，if就相当于一个return语句
    }
    else
    {
        auto trueBB=BasicBlock::create(module.get(),"",fun);
        auto mergeBB=BasicBlock::create(module.get(),"",fun);
        builder->create_cond_br(true_or_false,trueBB,mergeBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);
        if(builder->get_insert_block()->get_terminator()==nullptr)
            builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    }
}

void CminusfBuilder::visit(ASTIterationStmt &node) { 
    auto fun=builder->get_insert_block()->get_parent();
    
    auto loopBB=BasicBlock::create(module.get(),"",fun);
    auto conditionBB=BasicBlock::create(module.get(),"",fun);
    auto outloopBB=BasicBlock::create(module.get(),"",fun);

    builder->create_br(conditionBB);

    builder->set_insert_point(conditionBB);
    node.expression->accept(*this);
    auto type_exp=expression->get_type();
    Value* true_or_false;
    if(type_exp->is_integer_type()==1)
    {//不知道如何处理bool形
        auto type_zero=Type::get_int32_type(module.get());
        true_or_false=builder->create_icmp_gt(expression,CONST_ZERO(type_zero));
    }
    else 
    {
        auto type_zero=Type::get_float_type(module.get());
        true_or_false=builder->create_fcmp_gt(expression,CONST_ZERO(type_zero));
    }
    //由于expression只有int和float两种，故通过区分并与0比较确定为真or假
    builder->create_cond_br(true_or_false,loopBB,outloopBB);

    builder->set_insert_point(loopBB);
    node.statement->accept(*this);
    if(builder->get_insert_block()->get_terminator()==nullptr)
        builder->create_br(conditionBB);
    
    builder->set_insert_point(outloopBB);
}

void CminusfBuilder::visit(ASTReturnStmt &node) {
    if(node.expression==nullptr)
    {
        builder->create_void_ret();
    }
    else
    {
        node.expression->accept(*this);
        auto fun=builder->get_insert_block()->get_parent();
        auto rettype=fun->get_return_type();
        if(rettype->is_integer_type())
        {
            if(expression->get_type()->is_integer_type()!=1)
                expression=builder->create_fptosi(expression,Type::get_int32_type(module.get()));
            builder->create_ret(expression);
        }
        else if(rettype->is_float_type())
        {
            if(expression->get_type()->is_float_type()!=1)
                expression=builder->create_sitofp(expression,Type::get_float_type(module.get()));
            builder->create_ret(expression);
        }
        else if(rettype->is_void_type())
            builder->create_void_ret();
    }
  
 }


void CminusfBuilder::visit(ASTVar &node) {
    //ASTVar用来取值，你可以直接拿来用，对于赋值我会单独处理
    if(node.expression==nullptr)
    {
        auto var=scope.find(node.id);
        /*if(var->get_type()->is_array_type())
        {
            var=builder->create_gep(var,{ConstantInt::get(0, module.get()),ConstantInt::get(0, module.get())});
        }
        else */
        if(var->get_type()->is_pointer_type())
        {
            if(var->get_type()->get_pointer_element_type()->is_array_type())
            {
                //var=builder->create_gep(var,{ConstantInt::get(0, module.get())});
                var=builder->create_gep(var,{ConstantInt::get(0, module.get()),ConstantInt::get(0, module.get())});
            }
            else 
                var=builder->create_gep(var,{ConstantInt::get(0, module.get())});
        }
        //如果获得的id对应一个数组，说明是函数调用时作为实参要获得
        //该数组的指针，故会进行如上操作
        //一般情况下，都是获得一个变量的地址
        var_location=var;
        //找到变量所在的地址
        expression=builder->create_load(var_location);
        //获得该变量所存的值
    }
    else
    {
        node.expression->accept(*this);
        auto fun=builder->get_insert_block()->get_parent();
        Value* mark;
        if(expression->get_type()->is_float_type())
            mark=builder->create_fptosi(expression,Type::get_int32_type(module.get()));
        else 
            mark=expression;
        //获得i32类型的下标
        auto mark_check=builder->create_icmp_lt(mark,CONST_ZERO(Type::get_int32_type(module.get())));
        auto exceptBB=BasicBlock::create(module.get(),"",fun);
        auto correctBB=BasicBlock::create(module.get(),"",fun);
        builder->create_cond_br(mark_check,exceptBB,correctBB);
        //将下标与零比较，如果小于0，则对br来说true，进入except部分
        builder->set_insert_point(exceptBB);
        auto except=scope.find("neg_idx_except");
        builder->create_call(except,{});
        builder->create_br(correctBB);

        builder->set_insert_point(correctBB);

        auto vararray=scope.find(node.id);
        Value* vararraygep;
        if(vararray->get_type()->get_pointer_element_type()->is_array_type())
        {   
            //vararraygep=builder->create_gep(vararray,{ConstantInt::get(0, module.get())});
            vararraygep=builder->create_gep(vararray,{ConstantInt::get(0, module.get()),mark});
            //printf("认为是个数组");
        }
        else
        {
            vararraygep=builder->create_gep(vararray,{mark});
            //printf("认为是个指针");
        }
        
        var_location=vararraygep;
        expression=builder->create_load(vararraygep);
    }
    
}

void CminusfBuilder::visit(ASTAssignExpression &node) { 
    Value* leftvar;
    Value* leftnum;
    node.var->accept(*this);
    leftvar=var_location;
    leftnum=expression;
    //获得赋值表达式左侧变量的地址
    node.expression->accept(*this);
    //获得将要存储的等号右值
    //auto leftnumtype=leftvar->get_type()->get_pointer_element_type();
    //if(leftnumtype->is_integer_type())
    //    expression=builder->create_zext(expression,Type::get_int32_type(module.get()));
    //else
    //    expression=builder->create_zext(expression,Type::get_float_type(module.get()));
    if(leftnum->get_type()!=expression->get_type())
        if(leftnum->get_type()->is_integer_type())
            expression=builder->create_fptosi(expression,Type::get_int32_type(module.get()));
        else if(leftnum->get_type()->is_float_type())
            expression=builder->create_sitofp(expression,Type::get_float_type(module.get()));
        
    //将右值强制类型转换为左值类型
    builder->create_store(expression,leftvar);
    //进行赋值，由于该表达式的求值结果为赋到leftvar里的值，
    //所以不用再取一次了，就是expression
}

void CminusfBuilder::visit(ASTSimpleExpression &node) {
    node.additive_expression_l->accept(*this);
    if(node.additive_expression_r != nullptr) {
        Value* lhs = expression;
        node.additive_expression_r->accept(*this);
        Value* rhs = expression;
        //分类讨论运算符左右类型
        if((lhs->get_type()->is_integer_type())&&(rhs->get_type()->is_integer_type())){
            switch (node.op) {
                case OP_LE:
                    expression = builder->create_icmp_le(lhs, rhs);
                    break;
                case OP_LT:
                    expression = builder->create_icmp_lt(lhs, rhs);
                    break;
                case OP_GT:
                    expression = builder->create_icmp_gt(lhs, rhs);
                    break;
                case OP_GE:
                    expression = builder->create_icmp_ge(lhs, rhs);
                    break;
                case OP_EQ:
                    expression = builder->create_icmp_eq(lhs, rhs);
                    break;
                case OP_NEQ:
                    expression = builder->create_icmp_ne(lhs, rhs);
                    break;
            }
        }
        else{//至少有一个float
            if(lhs->get_type()->is_integer_type())
                lhs=builder->create_sitofp(lhs,Type::get_float_type(module.get()));
            if(rhs->get_type()->is_integer_type())
                rhs=builder->create_sitofp(rhs,Type::get_float_type(module.get()));
            switch (node.op) {
                case OP_LE:
                    expression = builder->create_fcmp_le(lhs, rhs);
                    break;
                case OP_LT:
                    expression = builder->create_fcmp_lt(lhs, rhs);
                    break;
                case OP_GT:
                    expression = builder->create_fcmp_gt(lhs, rhs);
                    break;
                case OP_GE:
                    expression = builder->create_fcmp_ge(lhs, rhs);
                    break;
                case OP_EQ:
                    expression = builder->create_fcmp_eq(lhs, rhs);
                    break;
                case OP_NEQ:
                    expression = builder->create_fcmp_ne(lhs, rhs);
                    break;
            }
        }
        //强制类型转换为i32
        expression=builder->create_zext(expression,Type::get_int32_type(module.get()));
    }
}
void CminusfBuilder::visit(ASTAdditiveExpression &node) {
    if (node.additive_expression == nullptr) {
        node.term->accept(*this);
    }
    else {
        //处理加法和减法
        node.additive_expression->accept(*this);
        Value* lhs = expression;
        node.term->accept(*this);
        Value* rhs = expression;
        if((lhs->get_type()->is_integer_type())&&(rhs->get_type()->is_integer_type())){
            switch (node.op) {
                case OP_PLUS:
                    expression = builder->create_iadd(lhs, rhs);
                    break;
                case OP_MINUS:
                    expression = builder->create_isub(lhs, rhs);
                    break;
            }
        }
        else{
            if(lhs->get_type()->is_integer_type())
                lhs=builder->create_sitofp(lhs,Type::get_float_type(module.get()));
            if(rhs->get_type()->is_integer_type())
                rhs=builder->create_sitofp(rhs,Type::get_float_type(module.get()));
            switch (node.op) {
                case OP_PLUS:
                    expression = builder->create_fadd(lhs, rhs);
                    break;
                case OP_MINUS:
                    expression = builder->create_fsub(lhs, rhs);
                    break;
            }
        }
    }

}

void CminusfBuilder::visit(ASTTerm &node) {
    if (node.term == nullptr) {
        node.factor->accept(*this);
    }
    else {
        node.term->accept(*this);
        Value* lhs = expression;
        node.factor->accept(*this);
        Value* rhs = expression;
        //处理乘法和除法
        if((lhs->get_type()->is_integer_type())&&(rhs->get_type()->is_integer_type())){
            switch (node.op) {
                case OP_MUL:
                    expression = builder->create_imul(lhs, rhs);
                    break;
                case OP_DIV:
                    expression = builder->create_isdiv(lhs, rhs);
                    break;
            }
        }
        else{
            if(lhs->get_type()->is_integer_type())
                lhs=builder->create_sitofp(lhs,Type::get_float_type(module.get()));
            if(rhs->get_type()->is_integer_type())
                rhs=builder->create_sitofp(rhs,Type::get_float_type(module.get()));
            switch (node.op) {
                case OP_MUL:
                    expression = builder->create_fmul(lhs, rhs);
                    break;
                case OP_DIV:
                    expression = builder->create_fdiv(lhs, rhs);
                    break;
            }
        }
    }

}

void CminusfBuilder::visit(ASTCall &node) {
    auto func=scope.find(node.id);
    auto funtype=static_cast<FunctionType*>(func->get_type());
    std::vector<Value*> args;
    //存实参;
    int i=0;
    auto int_ptr_type=Type::get_int32_ptr_type(module.get());
    auto float_ptr_type=Type::get_float_ptr_type(module.get());
    for(auto arg : node.args)
    {
        arg->accept(*this);
        //访问该arg并且获得地址var_location和expression
        if(funtype->get_param_type(i)->is_pointer_type())
        {   //当形参为指针
            /*auto p_element_Type=funtype->get_param_type(i)->get_pointer_element_type();
            auto actual_element_Type=var_location->get_type()->get_pointer_element_type();
            if(p_element_Type!=actual_element_Type)
            {
                if(p_element_Type->is_integer_type())
                    builder->create_zext(var_location,int_ptr_type);
                else
                    builder->create_zext(var_location,float_ptr_type);
            }*/
            args.push_back(var_location);    
        }
        else 
        {//当该形参是一个浮点数或者整型数
            if(funtype->get_param_type(i)!=expression->get_type())
            //获得的数值与形参类型不一致
                if(funtype->get_param_type(i)->is_integer_type())
                    expression=builder->create_fptosi(expression,Type::get_int32_type(module.get()));
                else
                    expression=builder->create_sitofp(expression,Type::get_float_type(module.get()));  
            args.push_back(expression);
        }

        i=i+1;
    }
    auto call=builder->create_call(func,args);
    expression=call;
 }
