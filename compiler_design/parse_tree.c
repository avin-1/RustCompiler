/*
 * parse_tree.c
 * =============
 * Implementation of the Parse Tree (AST) module.
 * Tree is rendered via GtkTreeStore in the UI layer.
 */

#include "parse_tree.h"

static int global_node_id = 1;

/* ================================================================ */
/*  NODE  CREATION                                                  */
/* ================================================================ */

Node *createNode(const char *name, Node *left, Node *right)
{
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Error: out of memory in createNode\n");
        exit(1);
    }
    strncpy(n->label, name, 49);
    n->label[49] = '\0';
    n->id = global_node_id++;
    n->child_count = 0;
    
    /* Backwards compatibility layer for binary parser hooks */
    if (left)  n->children[n->child_count++] = left;
    if (right) n->children[n->child_count++] = right;
    
    return n;
}

Node *createLeaf(const char *name)
{
    return createNode(name, NULL, NULL);
}

/* ================================================================ */
/*  TREE  DISPLAY                                                   */
/* ================================================================ */

void printTree(Node *node, int indent)
{
    if (node == NULL) return;

    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s\n", node->label);

    for (int i = 0; i < node->child_count; i++) {
        printTree(node->children[i], indent + 1);
    }
}

/* ================================================================ */
/*  TREE  FILE  OUTPUT                                              */
/* ================================================================ */

static void saveTreeHelper(Node *node, int indent, FILE *fp)
{
    if (node == NULL) return;

    for (int i = 0; i < indent; i++) fprintf(fp, "  ");
    fprintf(fp, "%s\n", node->label);

    for (int i = 0; i < node->child_count; i++) {
        saveTreeHelper(node->children[i], indent + 1, fp);
    }
}

void saveTree(Node *node, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    fprintf(fp, "=============================================================\n");
    fprintf(fp, "                    PARSE TREE OUTPUT                        \n");
    fprintf(fp, "=============================================================\n");

    saveTreeHelper(node, 0, fp);

    fprintf(fp, "=============================================================\n");
    fclose(fp);
}

/* ================================================================ */
/*  TREE  MEMORY  CLEANUP                                           */
/* ================================================================ */

void freeTree(Node *node)
{
    if (node == NULL) return;
    for (int i = 0; i < node->child_count; i++) {
        freeTree(node->children[i]);
    }
    free(node);
}
