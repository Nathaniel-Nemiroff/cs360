//write tis to print group descriptor information


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ext2type.h"
//#include "super.c"

typedef unsigned int u32;

// define shorter 
typedef struct ext2_group_desc  GD;
//typedef struct ext2_super_block SUPER;
//typedef struct ext2_inode 	INODE;
//typedef struct ext2_dir_entry_  DIR;

extern GD *gp;
//SUPER	*sp;
//INODE	*ip;
//DIR	*dp;

#define BLKSIZE 1024

char buf[BLKSIZE];
int fd;
/*
int get_group(int fd, int blk, char buf[])
{
   lseek(fd, (long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}
*/
gd()
{
   /*
   // read super block
   get_block(fd, 1, buf);
   sp = (SUPER *)buf;


   // check for EXT2

   printf("s_magic = %x\n", sp->magic);
   if(sp->s_magic != 0xEF53){
	printf("NOT an EXT2 FS\n");
	exit(1);
   }

   printf("EXT2 FS OK\n");
   */

   // read 0h group
   get_block(fd, 2, buf);
   gp = (GD *)buf;

   printf("bg_block_bitmap       = %d\n", gp->bg_block_bitmap);
   printf("bg_inode_bitmap       = %d\n", gp->bg_inode_bitmap);
   printf("bg_inode_table        = %d\n", gp->bg_inode_table);
   printf("bg_free_blocks_count  = %d\n", gp->bg_free_blocks_count);
   printf("bg_free_inodes_count  = %d\n", gp->bg_free_inodes_count);
   printf("bg_used_dirs_count    = %d\n", gp->bg_used_dirs_count);
}
