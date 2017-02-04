//dir_entries of the root directory.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ext2type.h"

#define BLKSIZE 1024


// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

extern int fd;
extern int debug;
int iblock;
int dblock;

/*
int get_block(int fd, int blk, char buf[])
{
   lseek(fd, (long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}
*/

dirnode(INODE *ip)
{
   char buf[BLKSIZE];


   //printf("ip->i_size = %d\n", ip->i_size);
   dblock = ip->i_block[0];

   get_block(fd, dblock, buf);
   //printf("ip->i_size = %d\n", ip->i_size);
   dp = (DIR *)buf;
   char *cp = dp;

   //printf("i_size = %d\n", ip->i_size);
   int remaining = ip->i_size;
   //printf("remaining space = %d\n", remaining);

   while(remaining > 0)
   {
	/*
	printf("inode     = %d\n", dp->inode);
	printf("rec_len   = %d\n", dp->rec_len);
	printf("name_len  = %d\n", dp->name_len);
	printf("file_type = %d\n", dp->file_type);
	*/
	printf("||%d", dp->inode);
	printf("|%d", dp->rec_len);
	printf("|%d", dp->name_len);
	printf("|%d", dp->file_type);
	printf("|name = ");
	int c;
	for(c = 0; c < dp->name_len; c++)
	   putchar(dp->name[c]);
	printf(" ...\n");

	remaining -= dp->rec_len;
	//printf("remaining space = %d\n", remaining);

	cp += dp->rec_len;
	dp = (DIR *)cp;
   }
}
dir()
{
   char *buf[BLKSIZE];
    // read GD
   get_block(fd, 2, buf);
   gp = (GD *)buf;

   int iblock = gp->bg_inode_table; // get inode start block num
   //printf("inode_block=%d\n", iblock);

   // get inode start block
   get_block(fd, iblock, buf);

   //printf("ip->i_size = %d\n", ip->i_size);
   ip = (INODE *)buf + 1; // ip points at 2nd INODE 

   dirnode(ip); 
}
