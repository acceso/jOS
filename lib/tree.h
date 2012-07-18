#ifndef LIB_TREE_H
#define LIB_TREE_H


#include <stdint.h>


struct tnode {
	/* We can refer to the child nodes by name or by index.
	 * The second way, although not intuitive, simplifies some code. */
	union {
		struct {
			struct tnode *l, *r;
		};
		struct tnode *n[2];
	};
	struct tnode *parent;
};


typedef s8 (*tree_cmp_fcn_t) (const struct tnode *, const struct tnode *);



struct tree {
	struct tnode *root;
	tree_cmp_fcn_t f_cmp;
};


struct tnode *tree_first (struct tree *tree);
struct tnode *tree_last (struct tree *tree);
struct tnode *tree_prev (struct tnode *node);
struct tnode *tree_next (struct tnode *node);

u8 tree_detach (struct tree *tree, struct tnode *node);
struct tnode *tree_lookup (const struct tree *tree, const struct tnode *key);
u8 tree_attach (struct tree *tree, struct tnode *node);
void tree_init (struct tree *tree, tree_cmp_fcn_t f_cmp);



#endif /* LIB_TREE_H */


