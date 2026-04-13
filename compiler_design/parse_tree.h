#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Node — a single node in the parse tree.
 * Supports n-ary structure for AST rendering.
 */
typedef struct Node {
    char         label[50];  /* label for this node */
    int          id;         /* legacy compat */
    struct Node *children[10];
    int          child_count;
} Node;

/* ================================================================ */
/*  PUBLIC API                                                      */
/* ================================================================ */

/*
 * createNode
 * ----------
 * Allocate a new internal node with the given label and children.
 */
Node *createNode(const char *name, Node *left, Node *right);

/*
 * createLeaf
 * ----------
 * Allocate a new leaf node (terminal) with the given label.
 */
Node *createLeaf(const char *name);

/*
 * printTree
 * ---------
 * Print the syntactic tree recursively to stdout using indentation.
 */
void printTree(Node *node, int indent);

/*
 * saveTree
 * --------
 * Output the tree to a file stream.
 */
void saveTree(Node *node, const char *filename);

/*
 * freeTree
 * --------
 * Post-order traversal to free dynamically allocated tree memory.
 */
void freeTree(Node *node);

#endif // PARSE_TREE_H
