// deallocate an Inode from the disk and release it from the imap

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ext2type.h"

// define shorter TYPES
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024

extern GD    *gp;
extern SUPER *sp;
extern INODE *ip;
extern DIR   *dp;

extern int fd;
extern int imap, bmap;
extern int ninodes, nblocks, nfreeInodes, nfreeBlocks;

/*
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

int tst_bit(char *buf, int bit)
{
   int i, j;
   i = bit/8; j=bit%8;
   if(buf[i] & (1<<j))
	return 1;
   return 0;
}
int set_bit(char *buf, int bit)
{
   int i, j;
   i = bit/8; j=bit%8;
   buf[i] |= (1<<j);
}
int clr_bit(char *buf, int bit)
{
   int i, j;
   i = bit/8; j=bit%8;
   buf[i] &= ~(1<<j);
}
*/

int incFreeInodes(int dev)
{
   char buf[BLKSIZE];

   // inc free inodes count in super and gd
   get_block(dev, 1, buf);
   sp = (SUPER *)buf;
   sp->s_free_inodes_count++;
   put_block(dev, 1, buf);

   get_block(dev, 2, buf);
   gp = (GD *)buf;
   gp->bg_free_inodes_count++;
   put_block(dev, 2, buf);
}

int idealloc(int dev)
{
   int i;
   char buf[BLKSIZE];

   // read inode_bitmap block
   get_block(dev, imap, buf);

   for(i=1; i<ninodes; i++)
   {
	if(tst_bit(buf, i)==0)
	{
	   clr_bit(buf, i-1);
	   incFreeInodes(dev);

	   put_block(dev, imap, buf);

	   return i;
	}
   }
   printf("idealloc(): no more used inodes\n");
   return 0;
}

/*
char *disk = "mydisk";

main(int argc, char *argv[])
{
   int i, ino, bno;
   char buf[BLKSIZE];

   if(argc > 1)
	disk = argv[1];

   fd = open(disk, O_RDWR);
   if(fd < 0)
   {
	printf("open %s failed\n", disk);
	exit(1);
   }

   get_block(fd, 1, buf);
   sp = (SUPER *)buf;

   ninodes = sp->s_inodes_count;
   nblocks = sp->s_inodes_count;
   nfreeInodes = sp->s_free_inodes_count;
   nfreeBlocks = sp->s_free_blocks_count;
   printf("ninodes=%d nblocks =%d nfreeInodes =%d nfreeBlocks=%d\n",
	ninodes, nblocks, nfreeInodes, nfreeBlocks);

   get_block(fd, 2, buf);
   gp = (GD *)buf;

   imap = gp->bg_inode_bitmap;
   printf("imap =%d\n", imap);
   bmap = gp->bg_block_bitmap;
   printf("bmap =%d\n", bmap);
   getchar();

   for(i=0; i<5; i++)
   {
	ino=idealloc(fd);
	printf("deallocated ino = %d\n", ino);
	//bno = bdealloc(fd);
	//printf("deallocated bno = %d\n", bno);
   }
}
*/
