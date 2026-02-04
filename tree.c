#include <stddef.h>
#include "tree.h"

#define VOID_TO_STR(DATA) ((char*) DATA)

#define PRINT_STR_NODE(FILE, PARENT_VAL, CHILD_VAL) fprintf(FILE, "%s -> %s;\n", VOID_TO_STR(PARENT_VAL), VOID_TO_STR(CHILD_VAL))
#define PRINT_NULL_NODE(FILE, PARENT_VAL, NULL_COUNT) fprintf(FILE, "null%lu [shape=point];\n", ++NULL_COUNT), \
    fprintf(FILE, "%s -> null%lu;\n", VOID_TO_STR(PARENT_VAL), NULL_COUNT)

void print_tree(FILE *f, node_t root)
{
    static size_t null_count = 0;
    if (!root)
        return;

    if (root->left)
        PRINT_STR_NODE(f, root->data, root->left->data);
    else
        PRINT_NULL_NODE(f, root->data, null_count);
    
    if (root->right)
        PRINT_STR_NODE(f, root->data, root->right->data);
    else
        PRINT_NULL_NODE(f, root->data, null_count);

    print_tree(f, root->left);
    print_tree(f, root->right);
}

void clear_tree(node_t root)
{
    if (!root)
        return;
    while (root)
    {
        node_t tmp;
        if (root->left)
        {
            tmp = root->left;
            root->left = NULL;
            root = tmp;
        }
        else if (root->right)
        {
            tmp = root->right;
            root->right = NULL;
            root = tmp;
        }
        else
        {
            tmp = root->parent;
            free_node(root);
            root = tmp;
        }
    }
}

void print_tree_prefix(node_t root)
{
    if (!root)
        return;

    if (!root->parent)
        printf("[ ");

    printf("'%s' ", VOID_TO_STR(root->data));
    print_tree_prefix(root->left);
    print_tree_prefix(root->right);
    
    if (!root->parent)
        printf("]\n");
}

void print_tree_infix(node_t root)
{
    if (!root)
        return;

    if (!root->parent)
        printf("[ ");

    print_tree_infix(root->left);
    printf("'%s' ", VOID_TO_STR(root->data));
    print_tree_infix(root->right);

    if (!root->parent)
        printf("]\n");
}

void print_tree_postfix(node_t root)
{
    if (!root)
        return;

    if (!root->parent)
        printf("[ ");

    print_tree_postfix(root->left);
    print_tree_postfix(root->right);
    printf("'%s' ", VOID_TO_STR(root->data));

    if (!root->parent)
        printf("]\n");
}

node_t find_in_tree(node_t root, void *data, int (*cmp)(const void*, const void*))
{
    if (!root)
        return NULL;
    while (root)
    {
        int cmp_res = cmp(root->data, data);
        if (cmp_res < 0)
            root = root->left;
        else if (cmp_res > 0)
            root = root->right;
        else
            return root;
    }
    return NULL;
}

node_t insert_node(node_t root, node_t node, int (*cmp)(const void*, const void*))
{
    if (!root)
        return node;

    node_t parent;
    int cmp_res;
    while (root)
    {
        parent = root;
        cmp_res = node_cmp(root, node, cmp);
        if (cmp_res < 0)
            root = root->right;
        else if (cmp_res > 0)
            root = root->left;
        else
            return NULL;
    }

    root = parent;
    if (cmp_res < 0)
        root->right = node;
    else
        root->left = node;
    node->parent = root;

    while (root->parent)
        root = root->parent;
    return root;
}

node_t remove_node(node_t root, node_t node, int (*cmp)(const void*, const void*))
{
    int cmp_res;
    while (root && (cmp_res = node_cmp(root, node, cmp)) != 0)
    {
        if (cmp_res < 0)
            root = root->right;
        else
            root = root->left;
    }

    if (!root)
        return NULL;

    if (root->left && root->right)
    {
        node_t left = root->left, right = root->right;
        left->parent = root->parent;
        if (left->parent)
        {
            if (node_cmp(left->parent, left, cmp) < 0)
                left->parent->right = left;
            else
                left->parent->left = left;
        }

        free_node(root);
        right->parent = left;
        root = left->right;
        left->right = right;

        while (right->left)
            right = right->left;
        right->left = root;
        if (root)
            root->parent = right;

        root = left;
    }
    else if (root->left || root->right)
    {
        node_t child = root->left ? root->left : root->right;
        if (root->parent)
        {
            if (node_cmp(root->parent, root, cmp) > 0)
                root->parent->left = child;
            else
                root->parent->right = child;
        }
        child->parent = root->parent;
        free_node(root);

        root = child;
    }
    else
    {
        if (root->parent)
        {
            node_t parent = root->parent;
            if (parent->left == root)
                parent->left = NULL;
            else
                parent->right = NULL;
            free_node(root);
            root = parent;
        }
        else
        {
            free_node(root);
            return NULL;
        }
    }

    while (root->parent)
        root = root->parent;
    return root;
}
