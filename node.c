#include <stdlib.h>
#include <string.h>
#include "node.h"

node_t create_node(const void *data, size_t datasize)
{
	node_t node;

	node = malloc(sizeof(*node));
	if (node)
	{
        node->parent = NULL;
        node->left = NULL;
        node->right = NULL;
		node->data = malloc(datasize);
        if (!node->data)
        {
            free(node);
            node = NULL;
        }
        else
            memcpy(node->data, data, datasize);
	}

	return node;
}

void free_node(node_t node)
{
    free(node->data);
	free(node);
}

int node_cmp(const node_t l, const node_t r, int (*cmp)(const void*, const void*))
{
    return cmp(l->data, r->data);
}
