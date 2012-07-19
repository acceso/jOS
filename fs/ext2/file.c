
#include <stdint.h>
#include <stdio.h>

#include <drivers/block.h>

#include <fs/fs.h>



struct bhead *
ext2_bread (struct super *sb, size_t bnum)
{
	return bread (sb, bnum);
}



