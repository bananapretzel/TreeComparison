#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "mylib.h"

#define IS_BLACK(x) ((NULL == (x)) || (BLACK == (x)->colour))
#define IS_RED(x) ((NULL != (x)) && (RED == (x)->colour))

static tree_t tree_type;

struct tree_node {
    char *key;
    tree_colour colour;
    tree parent;
    tree left;
    tree right;
    int frequency;
};

static void tree_output_dot_aux(tree t, FILE *out);

/**
 * Rotate a tree right around the root node. The root node becomes the
 * the node that was previously the left node.
 *
 * @param t the tree to be rotated right.
 * @return the new tree now right rotated.
 */
tree right_rotate(tree t) {
    tree x = t->left;
    tree y = x->right;
    x->right = t;
    t->left = y;
    t->parent = x;
    if (y != NULL) {
        y->parent = t;
    }
    return x;
}

/**
 * Rotate a tree left around the root node. The root node becomes the
 * the node that was previously the right node.
 *
 * @param t the tree to be rotated left.
 * @return the new tree now left rotated.
 */
tree left_rotate(tree t) {
    tree x = t->right;
    tree y = x->left;
    x->left = t;
    t->right = y;
    t->parent = x;
    if (y != NULL) {
        y->parent = t;
    }
    return x;
}

/**
 * Flip the colours of the root node and it's immediate leaves.
 *
 * @param t the tree to have it's colours flipped.
 * @return the new tree with  it's colours flipped.
 */
tree flipColour(tree t) {
    t->colour = RED;
    t->left->colour = BLACK;
    t->right->colour = BLACK;
    return t;
}

/**
 * Free the memory being used by a tree structure.
 *
 * @param t the tree to free from memory.
 */
void tree_free(tree t) {
    if (t == NULL) {
        return;
    }
    tree_free(t->right);
    tree_free(t->left);
    free(t->key);
    free(t);
}

/**
 * Print the inorder traversal of a tree. A helper function is used
 * to format the output.
 *
 * @param t the tree to print the inorder traversal of.
 * @param f a function to format the output.
 */
void tree_inorder(tree t, void f(int freq, char *str)) {
    if (t == NULL) {
        return;
    }
    if (t->left != NULL) {
        tree_inorder(t->left, f);
    }
    if (t->key != NULL) {
        f(t->frequency, t->key);
    }
    if (t->right != NULL) {
        tree_inorder(t->right, f);
    }
}

/**
 * Print the preorder traversal of a tree. A helper function is used
 * to format the output.
 *
 * @param t the tree to print the preorder traversal of.
 * @param f a function to format the output.
 */
void tree_preorder(tree t, void f(int freq, char *str)) {
    if (t == NULL) {
        return;
    }
    if (t->key != NULL) {
        /* Test code fixing root node always black */
        printf("Colour: ");
        if (t->colour == RED) {
            printf("RED\t");
        } else {
            printf("BLACK\t");
        }
        f(t->frequency, t->key);
    }
    if (t->left != NULL) {
        tree_preorder(t->left, f);
    }
    if (t->right != NULL) {
        tree_preorder(t->right, f);
    }
}

/**
 * Inserts a new word into the tree based on the first non-matching character in
 * the string when comparing the insert word to the current nodes word
 * (look up strcmp if not clear). If the tree is a red-black tree then a
 * re-balancing function is also called.
 *
 * @param t the tree to insert the string into.
 * @param str the string to insert in the tree.
 * @return the tree with the new string now inserted.
 */
tree tree_insert(tree t, char *str)
    {
    int comparison;
    if (t == NULL || t->key == NULL) {
        t = emalloc(sizeof *t);
        t->colour = RED;
        t->frequency = 1;
        t->key = emalloc((strlen(str) + 1) * sizeof(char));
        t->left = NULL;
        t->right = NULL;
        t->parent = NULL;
        strcpy(t->key, str);
        return t;
    }
    comparison = strcmp(t->key, str);
    if (comparison < 0) {
        t->right = tree_insert(t->right, str);
        t->right->parent = t;
    } else if (comparison > 0) {
        t->left = tree_insert(t->left, str);
        t->left->parent = t;
    } else if (comparison == 0) {
        t->frequency++;
        return t;
    }

    if (tree_type == RBT) {
        t = tree_fix(t);
    }

    return t;
}

tree root_check(tree t) {
    if IS_RED(t) {
        t->colour = BLACK;
        return t;
    }
    return t;
}

/**
 * Constructor for a tree structure.
 *
 * @param type the type of tree (red-black or binary search).
 * @return the tree structure just created.
 */
tree tree_new(tree_t type) {
    tree_type = type;
    return NULL;
}

/**
 * Check if a tree has a specific string. Return 1 if yes, 0 if no.
 *
 * @param t the tree to search for the string value.
 * @param str the string to search for in the tree.
 * @return integer value that represents whether the string was found.
 */
int tree_search(tree t, char *str) {
    if (t == NULL) {
        return 0;
    }
    if (strcmp(t->key, str) == 0) {
        return 1;
    } else if (strcmp(t->key, str) < 0) {
        return tree_search(t->right, str);
    } else {
        return tree_search(t->left, str);
    }
}

/**
 * Check an input tree for breaking the rules of red-black trees.
 * If it needs changed perform the nessacery rotations and
 * colour changes to satisfy the requirements of a red-black tree.
 *
 * @param t the tree to to be fixed.
 * @return the tree that has been fixed.
 */
tree tree_fix(tree t) {

    if (t == NULL) {
        return NULL;
    }
//    if (t->parent == NULL) {
//        t->colour = BLACK;
//    }

    if (IS_RED(t->left) && IS_RED(t->left->left) && IS_RED(t->right)) {
        t = flipColour(t);
    }
    if (IS_RED(t->left) && IS_RED(t->left->left) && IS_BLACK(t->right)) {
        t = right_rotate(t);
        t->right->parent = t;
        t->parent = NULL;
        t->colour = BLACK;
        t->right->colour = RED;
        t->parent = NULL;
    }

    if (IS_RED(t->left) && IS_RED(t->left->right) && IS_RED(t->right)) {
        t = flipColour(t);
    }
    if (IS_RED(t->left) && IS_RED(t->left->right) && IS_BLACK(t->right)) {
        t->left = left_rotate(t->left);
        t->left->parent = t;
        t->parent = NULL;
        t = right_rotate(t);
        t->parent = NULL;
        t->colour = BLACK;
        t->right->colour = RED;
    }

    if (IS_RED(t->right) && IS_RED(t->right->left) && IS_RED(t->left)) {
        t = flipColour(t);
    }
    if (IS_RED(t->right) && IS_RED(t->right->left) && IS_BLACK(t->left)) {
        t->right = right_rotate(t->right);
        t->right->parent = t;
        t->parent = NULL;
        t = left_rotate(t);
        t->parent = NULL;
        t->colour = BLACK;
        t->left->colour = RED;
    }

    if (IS_RED(t->right) && IS_RED(t->right->right) && IS_RED(t->left)) {
        t = flipColour(t);
    }

    if (IS_RED(t->right) && IS_RED(t->right->right) && IS_BLACK(t->left)) {
        t = left_rotate(t);
        t->left->parent = t;
        t->parent = NULL;
        t->colour = BLACK;
        t->left->colour = RED;
    }
    return t;
}

/**
 * Find the longest path from a tree's root node to leaf node
 * and return the integer value.
 *
 * @param t the tree to measure the depth of.
 * @return integer value of the tree depth.
 */
int tree_depth(tree t) {
    int left_height;
    int right_height;

    if (t == NULL) {
        return 0;
    }
    left_height = tree_depth(t->left);
    right_height = tree_depth(t->right);

    if (left_height > right_height) {
        return left_height + 1;
    } else {
        return right_height + 1;
    }
}

/**
 * Output a DOT description of this tree to the given output stream.
 * DOT is a plain text graph description language (see www.graphviz.org).
 * You can create a viewable graph with the command
 *
 *    dot -Tpdf < dotfile > dotfile.pdf
 *
 * where 'dotfile' is a file that has been written by tree_output_dot()
 *
 * You can also use png, ps, jpg, svg... instead of pdf
 *
 * @param t the tree to output the DOT description of.
 * @param out the stream to write the DOT description to.
 */
void tree_output_dot(tree t, FILE *out) {
    fprintf(out, "digraph tree {\nnode [shape = Mrecord, penwidth = 2];\n");
    tree_output_dot_aux(t, out);
    fprintf(out, "}\n");
}

/**
 * Traverses the tree writing a DOT description about connections, and
 * possibly colours, to the given output stream.
 *
 * @param t the tree to output a DOT description of.
 * @param out the stream to write the DOT output to.
 */
static void tree_output_dot_aux(tree t, FILE *out) {
    if (t == NULL) {
        return;
    }
    if (t->key != NULL) {
        fprintf(out, "\"%s\"[label=\"{<f0>%s:%d|{<f1>|<f2>}}\"color=%s];\n",
                t->key, t->key, t->frequency,
                (RBT == tree_type && RED == t->colour) ? "red" : "black");
    }
    if (t->left != NULL) {
        tree_output_dot_aux(t->left, out);
        fprintf(out, "\"%s\":f1 -> \"%s\":f0;\n", t->key, t->left->key);
    }
    if (t->right != NULL) {
        tree_output_dot_aux(t->right, out);
        fprintf(out, "\"%s\":f2 -> \"%s\":f0;\n", t->key, t->right->key);
    }
}
