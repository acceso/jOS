

#include <stdlib.h>

#include <lib/mem.h>

#include <lib/tree.h>

#include "test.h"


static struct tree tree;



#define URAMAX	20

struct ura {
	struct tnode tnode;
	u32 k;
	u32 v;
} ura[URAMAX];

struct ura ura2;


s8
f_cmp (const struct tnode *a, const struct tnode *b)
{
	struct ura *u1 = containerof (a, struct ura, tnode);
	struct ura *u2 = containerof (b, struct ura, tnode);

	if (u1->k < u2->k)
		return 1;
	else if (u1->k > u2->k)
		return -1;
	else
		return 0;
}



void
dump_tree (int dir)
{
	struct tnode *tn;

	if (dir == -1) {
		tn = tree_last (&tree);

		while (tn != NULL) {
			printf ("%d\n", ((struct ura *)tn)->k);
			tn = tree_prev (tn);
		}
	} else {
		tn = tree_first (&tree);

		while (tn != NULL) {
			printf ("%d\n", ((struct ura *)tn)->k);
			tn = tree_next (tn);
		}
	}

}


int
main (int argc, char **argv)
{
	int i;

	tree_init (&tree, f_cmp);

	for (i = 0; i < URAMAX; i++) {
		ura[i].k = i;
		ura[i].v = i * i;
	}

	tree_attach (&tree, &ura[5].tnode);
	tree_attach (&tree, &ura[2].tnode);
	tree_attach (&tree, &ura[9].tnode);
	tree_attach (&tree, &ura[1].tnode);
	tree_attach (&tree, &ura[3].tnode);
	tree_attach (&tree, &ura[4].tnode);
	tree_attach (&tree, &ura[8].tnode);
	tree_attach (&tree, &ura[6].tnode);
	tree_attach (&tree, &ura[7].tnode);
	tree_attach (&tree, &ura[11].tnode);
	tree_attach (&tree, &ura[10].tnode);
	tree_attach (&tree, &ura[12].tnode);

	tree_detach (&tree, &ura[10].tnode);

	dump_tree (1);


	return 0;
}





