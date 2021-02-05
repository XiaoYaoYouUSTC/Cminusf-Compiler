#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

#include <stdio.h>

#define SYNTAX_TREE_NODE_NAME_MAX 30

struct _syntax_tree_node {
	struct _syntax_tree_node * parent;
	struct _syntax_tree_node * children[10];
	int children_num;

	char name[SYNTAX_TREE_NODE_NAME_MAX];
};
typedef struct _syntax_tree_node syntax_tree_node;

syntax_tree_node * new_anon_syntax_tree_node();
syntax_tree_node * new_syntax_tree_node(const char * name);
int syntax_tree_add_child(syntax_tree_node * parent, syntax_tree_node * child);
void del_syntax_tree_node(syntax_tree_node * node, int recursive);

struct _syntax_tree {
	syntax_tree_node * root;
};
typedef struct _syntax_tree syntax_tree;

syntax_tree* new_syntax_tree();
void del_syntax_tree(syntax_tree * tree);
void print_syntax_tree(FILE * fout, syntax_tree * tree);

#endif /* SyntaxTree.h */
