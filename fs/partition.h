
#ifndef FS_PARTITION_H
#define FS_PARTITION_H


struct _partition {
	u64 base;
	u64 len;
	u8 flags;
	u8 type;
};

typedef struct _partition *partition;



void init_partition (void);

partition get_root_partition (void);
void partition_write (partition p, u64 offset, char *data, u64 len);


#endif /* FS_PARTITION_H */

