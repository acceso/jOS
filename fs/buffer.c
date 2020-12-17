
#include <stdint.h>

#include <lib/list.h>
#include <mm/mm.h>
#include <mm/kmalloc.h>
#include <fs/fs.h>

#include <drivers/block.h>



struct bhead *getblk(struct super *sb, size_t block)
{
	struct bhead *bh;

	list_for_each_entry(bh, &sb->bcache, l) {
		if (bh->bnum == block) {
			bh->count++;
			return bh;
		}
	}

	bh = xkmalloc(sizeof(struct bhead));

	bh->bnum = block;
	bh->data = xkmalloc(sb->blocksize);
	if (bh->data == NULL) {
		kfree(bh);
		return NULL;
	}
	bh->count = 0;
	list_add(&sb->bcache, &bh->l);

	return bh;
}



struct bhead *bread(struct super *sb, size_t block)
{
	struct bhead *bh;

	bh = getblk(sb, block);
	if (bh == NULL)
		return NULL;

	if (bh->count > 0)
		return bh;

	bh->count++;

	if (breadu(&sb->dev, block * sb->blocksize / sb->blocksizephys,
		bh->data, sb->blocksize) == 0)
		return NULL;

	return bh;
}




