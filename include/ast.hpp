#ifndef _SYNTAX_TREE_HPP_
#define _SYNTAX_TREE_HPP_
extern "C" {
#include "syntax_tree.h"
    extern syntax_tree *parse(const char *input);
}
#include <vector>
#include <memory>

enum CminusType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID
};

enum RelOp {
    // <=
    OP_LE,
    // <
    OP_LT,
    // >
    OP_GT,
    // >=
    OP_GE,
    // ==
    OP_EQ,
    // !=
    OP_NEQ
};

enum AddOp {
    // +
    OP_PLUS,
    // -
    OP_MINUS
};

enum MulOp {
    // *
    OP_MUL,
    // /
    OP_DIV
};

class AST;

struct ASTNode;
struct ASTProgram;
struct ASTDeclaration;
struct ASTNum;
struct ASTVarDeclaration;
struct ASTFunDeclaration;
struct ASTParam;
struct ASTCompoundStmt;
struct ASTStatement;
struct ASTExpressionStmt;
struct ASTSelectionStmt;
struct ASTIterationStmt;
struct ASTReturnStmt;
struct ASTFactor;
struct ASTExpression;
struct ASTVar;
struct ASTAssignExpression;
struct ASTSimpleExpression;
struct ASTAdditiveExpression;
struct ASTTerm ;
struct ASTCall;

class ASTVisitor;

class AST {
public:
    AST() = delete;
    AST(syntax_tree *);
    AST(AST &&tree) {
        root = tree.root;
        tree.root = nullptr;
    };
    ASTProgram* get_root() { return root.get(); }
    void run_visitor(ASTVisitor& visitor);
private:
    ASTNode* transform_node_iter(syntax_tree_node *);
    std::shared_ptr<ASTProgram> root = nullptr;
};

struct ASTNode {
    virtual void accept(ASTVisitor &) = 0;
};

struct ASTProgram : ASTNode {
    virtual void accept(ASTVisitor &) override final;
    std::vector<std::shared_ptr<ASTDeclaration>>
        declarations;
};

struct ASTDeclaration: ASTNode {
    virtual void accept(ASTVisitor &) override;
    CminusType type;
    std::string id;
};

struct ASTFactor: ASTNode {
    virtual void accept(ASTVisitor &) override;
};

struct ASTNum: ASTFactor {
    virtual void accept(ASTVisitor &) override final;
    CminusType type;
    union {
        int i_val;
        float f_val;
    };
};

struct ASTVarDeclaration: ASTDeclaration {
    virtual void accept(ASTVisitor &) override final;
    CminusType type;
    std::shared_ptr<ASTNum> num;
};

struct ASTFunDeclaration: ASTDeclaration {
    virtual void accept(ASTVisitor &) override final;
    std::vector<std::shared_ptr<ASTParam>> params;
    std::shared_ptr<ASTCompoundStmt> compound_stmt;
};

struct ASTParam: ASTNode {
    virtual void accept(ASTVisitor &) override final;
    CminusType type;
    std::string id;
    // true if it is array param
    bool isarray;
};

struct ASTStatement : ASTNode {
    virtual void accept(ASTVisitor &) override;
};

struct ASTCompoundStmt: ASTStatement {
    virtual void accept(ASTVisitor&) override final;
    std::vector<std::shared_ptr<ASTVarDeclaration>> local_declarations;
    std::vector<std::shared_ptr<ASTStatement>> statement_list;
};

struct ASTExpressionStmt: ASTStatement { 
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTExpression> expression; 
};

struct ASTSelectionStmt: ASTStatement {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTExpression> expression; 
    std::shared_ptr<ASTStatement> if_statement;
    // should be nullptr if no else structure exists
    std::shared_ptr<ASTStatement> else_statement;
};

struct ASTIterationStmt: ASTStatement {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTExpression> expression; 
    std::shared_ptr<ASTStatement> statement;
};

struct ASTReturnStmt: ASTStatement {
    virtual void accept(ASTVisitor &) override final;
    // should be nullptr if return void
    std::shared_ptr<ASTExpression> expression; 
};

struct ASTExpression: ASTFactor {
    virtual void accept(ASTVisitor &) override;
};

struct ASTAssignExpression: ASTExpression {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTVar> var;
    std::shared_ptr<ASTExpression> expression;
};

struct ASTSimpleExpression: ASTExpression {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTAdditiveExpression> additive_expression_l;
    std::shared_ptr<ASTAdditiveExpression> additive_expression_r;
    RelOp op;
};

struct ASTVar: ASTFactor {
    virtual void accept(ASTVisitor &) override final;
    std::string id;
    // nullptr if var is of int type
    std::shared_ptr<ASTExpression> expression;
};

struct ASTAdditiveExpression: ASTNode {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTAdditiveExpression> additive_expression;
    AddOp op;
    std::shared_ptr<ASTTerm> term;
};

struct ASTTerm : ASTNode {
    virtual void accept(ASTVisitor &) override final;
    std::shared_ptr<ASTTerm> term;
    MulOp op;
    std::shared_ptr<ASTFactor> factor;
};

struct ASTCall: ASTFactor {
    virtual void accept(ASTVisitor &) override final;
    std::string id;
    std::vector<std::shared_ptr<ASTExpression>> args;
};

class ASTVisitor {
public:
    virtual void visit(ASTProgram &) = 0;
    virtual void visit(ASTNum &) = 0;
    virtual void visit(ASTVarDeclaration &) = 0;
    virtual void visit(ASTFunDeclaration &) = 0;
    virtual void visit(ASTParam &) = 0;
    virtual void visit(ASTCompoundStmt &) = 0;
    virtual void visit(ASTExpressionStmt &) = 0;
    virtual void visit(ASTSelectionStmt &) = 0;
    virtual void visit(ASTIterationStmt &) = 0;
    virtual void visit(ASTReturnStmt &) = 0;
    virtual void visit(ASTAssignExpression &) = 0;
    virtual void visit(ASTSimpleExpression &) = 0;
    virtual void visit(ASTAdditiveExpression &) = 0;
    virtual void visit(ASTVar &) = 0;
    virtual void visit(ASTTerm &) = 0;
    virtual void visit(ASTCall &) = 0;
};

class ASTPrinter : public ASTVisitor {
public:
    virtual void visit(ASTProgram &) override final;
    virtual void visit(ASTNum &) override final;
    virtual void visit(ASTVarDeclaration &) override final;
    virtual void visit(ASTFunDeclaration &) override final;
    virtual void visit(ASTParam &) override final;
    virtual void visit(ASTCompoundStmt &) override final;
    virtual void visit(ASTExpressionStmt &) override final;
    virtual void visit(ASTSelectionStmt &) override final;
    virtual void visit(ASTIterationStmt &) override final;
    virtual void visit(ASTReturnStmt &) override final;
    virtual void visit(ASTAssignExpression &) override final;
    virtual void visit(ASTSimpleExpression &) override final;
    virtual void visit(ASTAdditiveExpression &) override final;
    virtual void visit(ASTVar &) override final;
    virtual void visit(ASTTerm &) override final;
    virtual void visit(ASTCall &) override final;
    void add_depth() { depth += 2; }
    void remove_depth() { depth -= 2; }
private:
    int depth = 0;
};
#endif

