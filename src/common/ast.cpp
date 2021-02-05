#include "ast.hpp"
#include <cstring>
#include <stack>
#include <iostream>
#define _AST_NODE_ERROR_ \
    std::cerr << "Abort due to node cast error."\
    "Contact with TAs to solve your problem."\
    << std::endl;\
    std::abort();
#define _STR_EQ(a, b) (strcmp((a), (b)) == 0)

void AST::run_visitor(ASTVisitor &visitor) {
    root->accept(visitor);
}

AST::AST(syntax_tree* s) {
    if (s == nullptr) {
        std::cerr << "empty input tree!" << std::endl;
        std::abort();
    }
    auto node = transform_node_iter(s->root);
    del_syntax_tree(s);
    root = std::shared_ptr<ASTProgram>(
            static_cast<ASTProgram *>(node));
}

ASTNode *
AST::transform_node_iter(syntax_tree_node *n) {
    if (_STR_EQ(n->name, "program")) {
        auto node = new ASTProgram();

        // flatten declaration list
        std::stack<syntax_tree_node *> s;
        auto list_ptr = n->children[0];
        while (list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);

        while (!s.empty()) {
            auto child_node = static_cast<ASTDeclaration*>(
                    transform_node_iter(s.top()));

            auto child_node_shared =
                std::shared_ptr<ASTDeclaration>(child_node);
            node->declarations.push_back(child_node_shared);
            s.pop();
        }
        return node;
    } else if (_STR_EQ(n->name, "declaration")) {
        return transform_node_iter(n->children[0]);
    } else if (_STR_EQ(n->name, "var-declaration")) {
        auto node = new ASTVarDeclaration();

        if (_STR_EQ(n->children[0]->children[0]->name, "int"))
            node->type = TYPE_INT;
        else
            node->type = TYPE_FLOAT;
 
        if (n->children_num == 3) {
            node->id = n->children[1]->name;
        } else if (n->children_num == 6) {
            node->id = n->children[1]->name;
            int num = std::stoi(n->children[3]->name);
            auto num_node = std::make_shared<ASTNum>();
            num_node->i_val = num;
            num_node->type = TYPE_INT;
            node->num = num_node;
        } else {
            std::cerr << "[ast]: var-declaration transform failure!" << std::endl;
            std::abort();
        }
        return node;
    } else if (_STR_EQ(n->name, "fun-declaration")) {
        auto node = new ASTFunDeclaration();
        if (_STR_EQ(n->children[0]->children[0]->name, "int")) {
            node->type = TYPE_INT;
        } else if (_STR_EQ(n->children[0]->children[0]->name, "float")) {
            node->type = TYPE_FLOAT;
        } else {
            node->type = TYPE_VOID;
        }

        node->id = n->children[1]->name;

        // flatten params
        std::stack<syntax_tree_node *> s;
        auto list_ptr = n->children[3]->children[0];
        if (list_ptr->children_num != 0) {
            if (list_ptr->children_num == 3) {
                while (list_ptr->children_num == 3) {
                    s.push(list_ptr->children[2]);
                    list_ptr = list_ptr->children[0];
                }
            }
            s.push(list_ptr->children[0]);

            while (!s.empty()) {
                auto child_node = static_cast<ASTParam*>(
                        transform_node_iter(s.top()));

                auto child_node_shared =
                    std::shared_ptr<ASTParam>(child_node);
                node->params.push_back(child_node_shared);
                s.pop();
            }
        }

        auto stmt_node =
            static_cast<ASTCompoundStmt *>(
                    transform_node_iter(n->children[5]));
        node->compound_stmt = std::shared_ptr<ASTCompoundStmt>(stmt_node);
        return node;
    } else if (_STR_EQ(n->name, "param")) {
        auto node = new ASTParam();
        if (_STR_EQ(n->children[0]->children[0]->name, "int"))
            node->type = TYPE_INT;
        else
            node->type = TYPE_FLOAT;
        node->id = n->children[1]->name;
        if (n->children_num == 3)
            node->isarray = true;
        return node;
    } else if (_STR_EQ(n->name, "compound-stmt")) {
        auto node = new ASTCompoundStmt();
        if (n->children[1]->children_num == 2) {
            // flatten local declarations
            auto list_ptr = n->children[1];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num == 2) {
                s.push(list_ptr->children[1]);
                list_ptr = list_ptr->children[0];
            }

            while (!s.empty()) {
                auto decl_node =
                    static_cast<ASTVarDeclaration*>(
                            transform_node_iter(s.top()));
                auto decl_node_ptr = std::shared_ptr<ASTVarDeclaration>(decl_node);
                node->local_declarations.push_back(decl_node_ptr);
                s.pop();
            }
        }

        if (n->children[2]->children_num ==    2) {
            // flatten statement-list
            auto list_ptr = n->children[2];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num == 2) {
                s.push(list_ptr->children[1]);
                list_ptr = list_ptr->children[0];
            }

            while (!s.empty()) {
                auto stmt_node =
                    static_cast<ASTStatement *>(
                            transform_node_iter(s.top()));
                auto stmt_node_ptr = std::shared_ptr<ASTStatement>(stmt_node);
                node->statement_list.push_back(stmt_node_ptr);
                s.pop();
            }
        }
        return node;
    } else if (_STR_EQ(n->name, "statement")) {
        return transform_node_iter(n->children[0]);
    } else if (_STR_EQ(n->name, "expression-stmt")) {
        auto node = new ASTExpressionStmt();
        if (n->children_num == 2) {
            auto expr_node =
                static_cast<ASTExpression *>(
                        transform_node_iter(n->children[0]));

            auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
            node->expression = expr_node_ptr;
        }
        return node;
    } else if (_STR_EQ(n->name, "selection-stmt")) {
        auto node = new ASTSelectionStmt();

        auto expr_node =
            static_cast<ASTExpression *>(
                    transform_node_iter(n->children[2]));
        auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
        node->expression = expr_node_ptr;

        auto if_stmt_node =
            static_cast<ASTStatement *>(
                    transform_node_iter(n->children[4]));
        auto if_stmt_node_ptr = std::shared_ptr<ASTStatement>(if_stmt_node);
        node->if_statement = if_stmt_node_ptr;

        // check whether this selection statement contains
        // else structure
        if (n->children_num == 7) {
            auto else_stmt_node =
                static_cast<ASTStatement *>(
                        transform_node_iter(n->children[6]));
            auto else_stmt_node_ptr = std::shared_ptr<ASTStatement>(else_stmt_node);
            node->else_statement = else_stmt_node_ptr;
        }

        return node;
    } else if (_STR_EQ(n->name, "iteration-stmt")) {
        auto node = new ASTIterationStmt();

        auto expr_node =
            static_cast<ASTExpression *>(
                    transform_node_iter(n->children[2]));
        auto expr_node_ptr = std::shared_ptr<ASTExpression>(expr_node);
        node->expression = expr_node_ptr;

        auto stmt_node =
            static_cast<ASTStatement *>(
                    transform_node_iter(n->children[4]));
        auto stmt_node_ptr = std::shared_ptr<ASTStatement>(stmt_node);
        node->statement = stmt_node_ptr;

        return node;
    } else if (_STR_EQ(n->name, "return-stmt")) {
        auto node = new ASTReturnStmt();
        if (n->children_num == 3) {
            auto expr_node =
                static_cast<ASTExpression *>(
                        transform_node_iter(n->children[1]));
            node->expression =
                std::shared_ptr<ASTExpression>(expr_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "expression")) {
        // simple-expression
        if (n->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        auto node = new ASTAssignExpression();

        auto var_node =
            static_cast<ASTVar *>(
                    transform_node_iter(n->children[0]));
        node->var = std::shared_ptr<ASTVar>(var_node);

        auto expr_node =
            static_cast<ASTExpression *>(
                    transform_node_iter(n->children[2]));
        node->expression =
            std::shared_ptr<ASTExpression>(expr_node);

        return node;
    } else if (_STR_EQ(n->name, "var")) {
        auto node = new ASTVar();
        node->id = n->children[0]->name;
        if (n->children_num == 4) {
            auto expr_node =
                static_cast<ASTExpression *>(
                        transform_node_iter(n->children[2]));
            node->expression =
                std::shared_ptr<ASTExpression>(expr_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "simple-expression")) {
        auto node = new ASTSimpleExpression();
        auto expr_node_1 =
            static_cast<ASTAdditiveExpression *>(
                    transform_node_iter(n->children[0]));
        node->additive_expression_l =
            std::shared_ptr<ASTAdditiveExpression>(expr_node_1);

        if (n->children_num == 3) {
            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "<="))
                node->op = OP_LE;
            else if (_STR_EQ(op_name, "<"))
                node->op = OP_LT;
            else if (_STR_EQ(op_name, ">"))
                node->op = OP_GT;
            else if (_STR_EQ(op_name, ">="))
                node->op = OP_GE;
            else if (_STR_EQ(op_name, "=="))
                node->op = OP_EQ;
            else if (_STR_EQ(op_name, "!="))
                node->op = OP_NEQ;

            auto expr_node_2 =
                static_cast<ASTAdditiveExpression *>(
                        transform_node_iter(n->children[2]));
            node->additive_expression_r =
                std::shared_ptr<ASTAdditiveExpression>(expr_node_2);
        }
        return node;
    } else if (_STR_EQ(n->name, "additive-expression")) {
        auto node = new ASTAdditiveExpression();
        if (n->children_num == 3) {
            auto add_expr_node =
                static_cast<ASTAdditiveExpression *>(
                        transform_node_iter(n->children[0]));
            node->additive_expression =
                std::shared_ptr<ASTAdditiveExpression>(add_expr_node);

            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "+"))
                node->op = OP_PLUS;
            else if (_STR_EQ(op_name, "-"))
                node->op = OP_MINUS;

            auto term_node =
                static_cast<ASTTerm *>(
                        transform_node_iter(n->children[2]));
            node->term = std::shared_ptr<ASTTerm>(term_node);
        } else {
            auto term_node =
                static_cast<ASTTerm *>(
                        transform_node_iter(n->children[0]));
            node->term = std::shared_ptr<ASTTerm>(term_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "term")) {
        auto node = new ASTTerm();
        if (n->children_num == 3) {
            auto term_node =
                static_cast<ASTTerm *>(
                        transform_node_iter(n->children[0]));
            node->term =
                std::shared_ptr<ASTTerm>(term_node);

            auto op_name = n->children[1]->children[0]->name;
            if (_STR_EQ(op_name, "*"))
                node->op = OP_MUL;
            else if (_STR_EQ(op_name, "/"))
                node->op = OP_DIV;

            auto factor_node =
                static_cast<ASTFactor *>(
                        transform_node_iter(n->children[2]));
            node->factor = std::shared_ptr<ASTFactor>(factor_node);
        } else {
            auto factor_node =
                static_cast<ASTFactor *>(
                        transform_node_iter(n->children[0]));
            node->factor = std::shared_ptr<ASTFactor>(factor_node);
        }
        return node;
    } else if (_STR_EQ(n->name, "factor")) {
        int i = 0;
        if (n->children_num == 3)
            i = 1;
        auto name = n->children[i]->name;
        if (_STR_EQ(name, "expression") ||
                _STR_EQ(name, "var") ||
                _STR_EQ(name, "call"))
            return transform_node_iter(n->children[i]);
        else {
            auto num_node = new ASTNum();
            if (_STR_EQ(name, "integer")) {
                num_node->type = TYPE_INT;
                num_node->i_val = std::stoi(n->children[i]->children[0]->name);
            } else if (_STR_EQ(name, "float")) {
                num_node->type = TYPE_FLOAT;
                num_node->f_val = std::stof(n->children[i]->children[0]->name);
            } else {
                _AST_NODE_ERROR_
            }
            return num_node;
        }
    } else if (_STR_EQ(n->name, "call")) {
        auto node = new ASTCall();
        node->id = n->children[0]->name;
        // flatten args
        if (_STR_EQ(n->children[2]->children[0]->name, "arg-list")) {
            auto list_ptr = n->children[2]->children[0];
            auto s = std::stack<syntax_tree_node *>();
            while (list_ptr->children_num == 3) {
                s.push(list_ptr->children[2]);
                list_ptr = list_ptr->children[0];
            }
            s.push(list_ptr->children[0]);

            while (!s.empty()) {
                auto expr_node =
                    static_cast<ASTExpression *>(
                            transform_node_iter(s.top()));
                auto expr_node_ptr =
                    std::shared_ptr<ASTExpression>(expr_node);
                node->args.push_back(expr_node_ptr);
                s.pop();
            }
        }
        return node;
    } else {
        std::cerr << "[ast]: transform failure!" << std::endl;
        std::abort();
    }
}

void ASTProgram::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTNum::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTVarDeclaration::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTFunDeclaration::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTParam::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTCompoundStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTExpressionStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTSelectionStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTIterationStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTReturnStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTAssignExpression::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTSimpleExpression::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTAdditiveExpression::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTVar::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTTerm::accept(ASTVisitor &visitor) { visitor.visit(*this); }
void ASTCall::accept(ASTVisitor &visitor) { visitor.visit(*this); }

void ASTFactor::accept(ASTVisitor &visitor) {
    auto expr =
        dynamic_cast<ASTExpression *>(this);
    if (expr) {
        expr->accept(visitor);
        return;
    }

    auto var =
        dynamic_cast<ASTVar *>(this);
    if (var) {
        var->accept(visitor);
        return;
    }

    auto call =
        dynamic_cast<ASTCall *>(this);
    if (call) {
        call->accept(visitor);
        return;
    }

    auto num =
        dynamic_cast<ASTNum *>(this);
    if (num) {
        num->accept(visitor);
        return;
    }

    _AST_NODE_ERROR_
}

void ASTDeclaration::accept(ASTVisitor &visitor) {
    auto var_decl =
        dynamic_cast<ASTVarDeclaration *>(this);
    if (var_decl) {
        var_decl->accept(visitor);
        return;
    }
    auto fun_decl =
        dynamic_cast<ASTFunDeclaration *>(this);
    if (fun_decl) {
        fun_decl->accept(visitor);
        return;
    }
    _AST_NODE_ERROR_
}

void ASTStatement::accept(ASTVisitor &visitor) {
    auto comp_stmt =
        dynamic_cast<ASTCompoundStmt *>(this);
    if (comp_stmt) {
        comp_stmt->accept(visitor);
        return;
    }

    auto expr_stmt =
        dynamic_cast<ASTExpressionStmt *>(this);
    if (expr_stmt) {
        expr_stmt->accept(visitor);
        return;
    }

    auto sele_stmt =
        dynamic_cast<ASTSelectionStmt *>(this);
    if (sele_stmt) {
        sele_stmt->accept(visitor);
        return;
    }

    auto iter_stmt =
        dynamic_cast<ASTIterationStmt *>(this);
    if (iter_stmt) {
        iter_stmt->accept(visitor);
        return;
    }

    auto ret_stmt =
        dynamic_cast<ASTReturnStmt *>(this);
    if (ret_stmt) {
        ret_stmt->accept(visitor);
        return;
    }
    _AST_NODE_ERROR_
}

void ASTExpression::accept(ASTVisitor &visitor) {
    auto simple_expr =
        dynamic_cast<ASTSimpleExpression *>(this);
    if (simple_expr) {
        simple_expr->accept(visitor);
        return;
    }

    auto assign_expr =
        dynamic_cast<ASTAssignExpression *>(this);
    if (assign_expr) {
        assign_expr->accept(visitor);
        return;
    }
    _AST_NODE_ERROR_
}

#define _DEBUG_PRINT_N_(N) {\
    std::cout << std::string(N, '-');\
}

void ASTPrinter::visit(ASTProgram &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "program" << std::endl;
    add_depth();
    for (auto decl: node.declarations) {
        decl->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTNum &node) {
    _DEBUG_PRINT_N_(depth);
    if (node.type == TYPE_INT) {
        std::cout << "num (int): " << node.i_val << std::endl;
    } else if (node.type == TYPE_FLOAT) {
        std::cout << "num (float): " << node.f_val << std::endl;
    } else {
        _AST_NODE_ERROR_
    }

}

void ASTPrinter::visit(ASTVarDeclaration &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "var-declaration: " << node.id;
    if (node.num != nullptr) {
        std::cout << "[]" << std::endl;
        add_depth();
        node.num->accept(*this);
        remove_depth();
        return;
    }
    std::cout << std::endl;
}

void ASTPrinter::visit(ASTFunDeclaration &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "fun-declaration: " << node.id << std::endl;
    add_depth();
    for (auto param: node.params) {
        param->accept(*this);
    }

    node.compound_stmt->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTParam &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "param: " << node.id;
    if (node.isarray)
        std::cout << "[]";
    std::cout << std::endl;
}

void ASTPrinter::visit(ASTCompoundStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "compound-stmt" << std::endl;
    add_depth();
    for (auto decl: node.local_declarations) {
        decl->accept(*this);
    }

    for (auto stmt: node.statement_list) {
        stmt->accept(*this);
    }
    remove_depth();
}

void ASTPrinter::visit(ASTExpressionStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "expression-stmt" << std::endl;
    add_depth();
    if (node.expression != nullptr)
        node.expression->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTSelectionStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "selection-stmt" << std::endl;
    add_depth();
    node.expression->accept(*this);
    node.if_statement->accept(*this);
    if (node.else_statement != nullptr)
    node.else_statement->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTIterationStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "iteration-stmt" << std::endl;
    add_depth();
    node.expression->accept(*this);
    node.statement->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTReturnStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "return-stmt";
    if (node.expression == nullptr) {
        std::cout << ": void" << std::endl;
    } else {
        std::cout << std::endl;
        add_depth();
        node.expression->accept(*this);
        remove_depth();
    }
}

void ASTPrinter::visit(ASTAssignExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "assign-expression" << std::endl;
    add_depth();
    node.var->accept(*this);
    node.expression->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTSimpleExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "simple-expression";
    if (node.additive_expression_r == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_LT) {
            std::cout << "<";
        } else if (node.op == OP_LE) {
            std::cout << "<=";
        } else if (node.op == OP_GE) {
            std::cout << ">=";
        } else if (node.op == OP_GT) {
            std::cout << ">";
        } else if (node.op == OP_EQ) {
            std::cout << "==";
        } else if (node.op == OP_NEQ) {
            std::cout << "!=";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    node.additive_expression_l->accept(*this);
    if (node.additive_expression_r != nullptr)
        node.additive_expression_r->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTAdditiveExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "additive-expression";
    if (node.additive_expression == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_PLUS) {
            std::cout << "+";
        } else if (node.op == OP_MINUS) {
            std::cout << "-";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    if (node.additive_expression != nullptr)
        node.additive_expression->accept(*this);
    node.term->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTVar &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "var: " << node.id;
    if (node.expression != nullptr) {
        std::cout << "[]" << std::endl;
        add_depth();
        node.expression->accept(*this);
        remove_depth();
        return;
    }
    std::cout << std::endl;
}

void ASTPrinter::visit(ASTTerm &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "term";
    if (node.term == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_MUL) {
            std::cout << "*";
        } else if (node.op == OP_DIV) {
            std::cout << "/";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    if (node.term != nullptr)
        node.term->accept(*this);

    node.factor->accept(*this);
    remove_depth();
}

void ASTPrinter::visit(ASTCall &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "call: " << node.id << "()" << std::endl;
    add_depth();
    for (auto arg: node.args) {
        arg->accept(*this);
    }
    remove_depth();
}
