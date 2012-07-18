

/* TODO: I want a red-black tree here. After many insert/remove ops,
 * our tree will no longer be O(log n)
 * 
 * TODO:
 * * we can cache the first and last entry (will speed up some operations
 *   but I don't need any of those operations right now).
 * * make tree_first, tree_last, tree_next, tree_prev have constant params.
 * 
 */


#include <stdint.h>

#include "tree.h"



/* Return the first (if dir == 0) or last (otherwise) nodes of a subtree */
static inline struct tnode *
tree_first_last (struct tnode *node, u8 dir)
{
	while (node->n[dir])
		node = node->n[dir];

	return node;
}



struct tnode *
tree_first (struct tree *tree)
{
	return tree_first_last (tree->root, 0);
}



struct tnode *
tree_last (struct tree *tree)
{
	return tree_first_last (tree->root, 1);
}



static inline struct tnode *
tree_prev_next (struct tnode *node, u8 dir)
{
	if (node->n[dir])
		return tree_first_last (node->n[dir], !dir);

	while (node->parent && node->parent->n[dir] == node) {
		if (node->parent == NULL)
			return NULL;

		node = node->parent;
	}

	return node->parent;
}



struct tnode *
tree_prev (struct tnode *node)
{
	return tree_prev_next (node, 0);
}



struct tnode *
tree_next (struct tnode *node)
{
	return tree_prev_next (node, 1);
}



/* It returns a node if found or NULL if not.
 * If parent != NULL, store a pointer to the parent there and returns in "pos"
 * if the node should be inserted right or left.
 */
static struct tnode *
lookup_node (const struct tree *tree, const struct tnode *node, 
		struct tnode **parent, u8 *pos)
{
	struct tnode *n = tree->root;
	struct tnode *p = NULL;
	u8 i = 0;
	s8 c;

	while (n) {
		p = n;

		c = tree->f_cmp (n, node);
		if (c < 0)
			i = 0;
		else if (c > 0)
			i = 1;
		else
			break;

		n = n->n[i];
	}


	if (parent != NULL)
		*parent = p;

	if (pos != NULL)
		*pos = i;


	return n;
}



static inline u8
is_leaf_node (const struct tnode *node)
{
	/* They can only point to the same place if they point to NULL. */
	return node->l == node->r;
}



/* Links parent[pos] and children */
static inline void
link_children_to_parent (struct tree *tree, struct tnode *node, u8 pos)
{
	if (node->parent) {
		node->parent->n[node->parent->l != node] = node->n[pos];
		/* This handles a NULL child */
		if (node->n[pos])
			node->n[pos]->parent = node->parent;
	} else {
		tree->root = node->n[pos];
		if (tree->root)
			node->n[pos]->parent = NULL;
	}
}



/* Dettach a node if exists.
 * Returns if it has been removed (1) or not (0). */
u8
tree_detach (struct tree *tree, struct tnode *node)
{
	struct tnode *n;
	u8 pos;
	
	n = lookup_node (tree, node, NULL, &pos);
	if (n == NULL)
		return 0;

	if (is_leaf_node (n))
		link_children_to_parent (tree, n, pos);
	else if (n->l == NULL || n->r == NULL)
		link_children_to_parent (tree, n, n->l == NULL);
	else if (n->l->r == NULL || n->r->l == NULL) {
		u8 pos = (n->l->r != NULL);
		link_children_to_parent (tree, n, pos);
		n->n[!pos]->parent = n->n[pos];
		n->n[pos]->n[!pos] = n->n[!pos];
	} else if (n->parent && n->parent->n[!pos] == NULL) {
		link_children_to_parent (tree, n, pos);
	} else {
		/* If we reach here, there are no shorcut, we need the next
		 * or the previous node. I'll leave it to pseudo-chance: */
		u8 pos = ((u64)n >> 6) & 1;
		struct tnode *new = tree_prev_next (n, pos);

		new->parent->n[!pos] = new->n[pos];
		new->l = n->l;
		new->r = n->r;
		new->parent = n->parent;
		n->l->parent = n->r->parent = new;

		if (n == tree->root)
			tree->root = new;
	}

	node->parent = node->l = node->r = NULL;

	return 1;
}



/* Searchs for a node. Returns the address if found and NULL if not found */
struct tnode *
tree_lookup (const struct tree *tree, const struct tnode *key)
{
	return lookup_node (tree, key, NULL, NULL);
}




/* Attach "node" to "tree".
 * Returns 0 if it is a new node and !0 if the node was already on the tree */
u8
tree_attach (struct tree *tree, struct tnode *node)
{
	struct tnode *n, *p;
	u8 pos;

	/* This node is already in the tree. */
	n = lookup_node (tree, node, &p, &pos);
	if (n != NULL)
		return 1;

	node->l = node->r = NULL;
	node->parent = p;

	if (p == NULL)
		tree->root = node;
	else
		/* Note pos has to be 0 or 1 if parent != NULL. */
		p->n[pos] = node;

	return 0;
}



void
tree_init (struct tree *tree, tree_cmp_fcn_t f_cmp)
{
	tree->root = NULL;
	tree->f_cmp = f_cmp;
}



