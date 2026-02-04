#ifndef TREE_H__
#define TREE_H__

#include <stdio.h>
#include "node.h"

void print_tree(FILE *f, node_t root);
void print_tree_prefix(node_t root);
void print_tree_infix(node_t root);
void print_tree_postfix(node_t root);
void clear_tree(node_t root);

node_t find_in_tree(node_t root, void *data, int (*cmp)(const void*, const void*));
node_t insert_node(node_t root, node_t node, int (*cmp)(const void*, const void*));
node_t remove_node(node_t root, node_t node, int (*cmp)(const void*, const void*));

#endif
