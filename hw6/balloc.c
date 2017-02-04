// write this to allocate a FREE disk block; return its bno

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ext2type.h"
//#include "ialloc.c"



// define shorter types
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024

extern GD    *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR   *dp;


/*
int fd;
int imap, bmap;
int ninodes, nblocks, nfreeInodes, nfreeBlocks;

int get_block(int fd, int blk, char buf[])
{
   lseek(fd, (long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}
int put_block(int fd, int blk, char buf[])
{
   lseek(fd, (long)blk*BLKSIZE, 0);
   write(fd, buf, BLKSIZE);
}
*/

extern int bmap;
extern int nblocks;

int decFreeBlocks(int dev)
{
   char buf[BLKSIZE];

   get_block(dev, 1, buf);
   sp = (SUPER *)buf;
   sp->s_free_blocks_count--;
   put_block(dev, 1, buf);

   get_block(dev, 2, buf);
   gp = (GD *)buf;
   gp->bg_free_blocks_count--;
   put_block(dev, 2, buf);
}

int balloc(int dev)
{
   int i;
   char buf[BLKSIZE];

   get_block(dev, bmap, buf);

   for(i=0; i<nblocks; i++)
   {
	if(tst_bit(buf,i)==0)
	{
	   set_bit(buf,i);
	   decFreeBlocks(dev);

	   put_block(dev, bmap, buf);

	   return i+1;
	}
   }
   printf("balloc(): no more free bnodes\n");
   return 0;
}


