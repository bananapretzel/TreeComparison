#ifndef TREE_H_
#define TREE_H_

typedef enum { RED, BLACK } tree_colour;
typedef enum tree_e { BST, RBT } tree_t;

typedef struct tree_node *tree;

extern tree right_rotate(tree t);
extern tree left_rotate(tree t);
extern tree flipColour(tree t);
extern void tree_free(tree t);
extern void tree_inorder(tree t, void f(int freq, char *str));
extern void tree_preorder(tree t, void f(int freq, char *str));
extern tree tree_insert(tree t, char *str);
extern tree tree_new(tree_t type);
extern int tree_search(tree t, char *str);
extern tree tree_fix(tree t);
extern int tree_depth(tree t);
extern void tree_output_dot(tree t, FILE *out);
extern tree root_check(tree t);

#endif
