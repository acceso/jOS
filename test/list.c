
#include <stdint.h>
#include <lib/list.h>

#include "test.h"


struct ura {
	struct list_head l;
	u8 val;
};


#define HOWMANY	5

int main(int argc, char **argv)
{
	struct ura ura[HOWMANY];
	struct ura *u;
	int i;

	list_init(&ura[0].l);

	for (i = 0; i < HOWMANY; i++) {
		ura[i].val = i;
		list_add(&ura[0].l, &ura[i].l);
	}


	list_for_each_entry(u, &ura[0].l, l)
		printf("%d\n", u->val);

	printf("%d\n", ura[0].val);

	return 0;
}


