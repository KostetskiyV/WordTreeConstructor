#ifndef NODE_H__
#define NODE_H__

#include <stddef.h>

typedef struct node* node_t;

struct node
{
    void *data;
    struct node *parent;
    struct node *left;
    struct node *right;
};

node_t create_node(const void *data, size_t datasize);
void free_node(node_t node);

int node_cmp(const node_t l, const node_t r, int (*cmp)(const void*, const void*));

#endif
