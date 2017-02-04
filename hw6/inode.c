// print information in INODE

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
int iblock;

/*
int get_block(int fd, int blk, char buf[])
{
   lseek(fd, (long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}
*/

inodedata(INODE *ip)
{
   printf("mode=%4x\n", ip->i_mode);
   printf("uid=%d\ngid=%d\n", ip->i_uid, ip->i_gid);
   printf("size=%d\n", ip->i_size);
   printf("time=%s", ctime(&ip->i_ctime));
   printf("link=%d\n", ip->i_links_count);
   printf("i_block[0]=%d\n", ip->i_block[0]);
   printf("i_blocks=%d\n", ip->i_blocks);

}
inode()
{
   char *buf[BLKSIZE];
   // read GD
   get_block(fd, 2, buf);
   gp = (GD *)buf;
   /***********************
   printf("%8d %8d %8d %8d %8d %8d\n",
	gp->bg_block_bitmap,
	gp->bg_inode_bitmap,
	gp->bg_inode_table,
	gp->bg_free_blocks_count,
	gp->bg_free_inodes_count,
	gp->bg_used_dirs_count);
   **********************/

   iblock = gp->bg_inode_table; // get inode start block num
   printf("inode_block=%d\n", iblock);

   // get indoe start block
   get_block(fd, iblock, buf);
   ip = (INODE *)buf + 1; // ip points at 2nd INODE

   inodedata(ip);
}

/*
char *disk = "mydisk";

main(int argc, char *argv[])
{
   if(argc > 1)
	disk = argv[1];

   fd = open(disk, O_RDONLY);
   if (fd < 0){
	printf("open %s failed\n", disk);
	exit(1);
   }

   inode();
   dir();
}
*/
