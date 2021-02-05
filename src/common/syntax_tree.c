#include <stdlib.h>
#include <string.h>

#include "syntax_tree.h"

syntax_tree_node * new_syntax_tree_node(const char * name)
{
	syntax_tree_node * new_node = (syntax_tree_node *)malloc(sizeof(syntax_tree_node));
	if (name)
		strncpy(new_node->name, name, SYNTAX_TREE_NODE_NAME_MAX);
	else
		new_node->name[0] = '\0';
	new_node->children_num = 0;
	return new_node;
}

int syntax_tree_add_child(syntax_tree_node * parent, syntax_tree_node * child)
{
	if (!parent || !child)	return -1;
	parent->children[parent->children_num++] = child;
	return parent->children_num;
}

void del_syntax_tree_node(syntax_tree_node * node, int recursive)
{
	if (!node)	return;

	int i;
	if (recursive) {
		for (i = 0; i < node->children_num; i++) {
			del_syntax_tree_node(node->children[i], 1);
		}
	}
	free(node);
}

syntax_tree * new_syntax_tree()
{
	return (syntax_tree *)malloc(sizeof(syntax_tree));
}

void del_syntax_tree(syntax_tree * tree)
{
	if (!tree)	return;

	if (tree->root) {
		del_syntax_tree_node(tree->root, 1);
	}
	free(tree);
}

void print_syntax_tree_node(FILE * fout, syntax_tree_node * node, int level)
{
	// assume fout valid now
	
	// check if "node" empty pointer
	if (!node)	return;
	
	// print myself
	int i;
	for (i = 0; i < level; i++) {
		fprintf(fout, "|  ");
	}
	fprintf(fout, ">--%s %s\n", (node->children_num ? "+" : "*"), node->name);

	for (i = 0; i < node->children_num; i++) {
		print_syntax_tree_node(fout, node->children[i], level + 1);
	}
}

void print_syntax_tree(FILE * fout, syntax_tree * tree)
{
	if (!fout)	return;
	
	print_syntax_tree_node(fout, tree->root, 0);
}

