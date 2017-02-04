#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ext2type.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

int fd;
int imap, bmap;
int ninodes, nblocks, nfreeInodes, nfreeBlocks;
int cwd;

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
   i=bit/8; j=bit%8;
   if(buf[i] & (1<<j))
	return 1;
   return 0;
}
int set_bit(char *buf, int bit)
{
   int i, j;
   i=bit/8; j=bit%8;
   buf[i] |= (1<<j);
}
int clr_bit(char *buf, int bit)
{
   int i, j;
   i=bit/8; j=bit%8;
   buf[i] &= ~(1<<j);
}


char *disk = "mydisk";
int debug;

main(int argc, char *argv[])
{
   int i, ino, bno;
   char buf[BLKSIZE];
   debug = 0;

   if(argc < 3)
   {
	printf("Usage: showblock DEVICE PATHNAME\n");
	return 0;
   }
   if(argc > 3)
	if(!strcmp(argv[3], "-d"))
	   debug = 1;

   disk = argv[1];

   fd = open(disk, O_RDWR);
   if(fd < 0)
   {
	printf("Failed to open disk %s\n", disk);
	exit(1);
   }

   // read SUPER block
   get_block(fd, 1, buf);
   sp = (SUPER *)buf;

   ninodes = sp->s_inodes_count;
   ninodes = sp->s_blocks_count;
   nfreeInodes = sp->s_free_inodes_count;
   nfreeBlocks = sp->s_free_blocks_count;
   if(debug)
	printf(" ninodes     =%d\n nblocks     =%d\n nfreeInodes=%d\n nfreeBlocks=%d\n", ninodes, nblocks, nfreeInodes, nfreeBlocks);

   // read GD0
   get_block(fd, 2, buf);
   gp = (GD *)buf;

   imap = gp->bg_inode_bitmap;
   bmap = gp->bg_block_bitmap;
   if(debug){
      printf("imap = %d\n", imap);
      printf("bmap = %d\n", bmap);
      getchar();
   }

   int iblock = gp->bg_inode_table;
   get_block(fd, iblock, buf);
   ip = (INODE *)buf+1;

   if(debug)
      dir(ip);

   //char *dbuf[BLKSIZE];
   dp = search(ip, argv[2]);
   ino = dp->inode;
   if(debug)
      printf("ino = %d\n", ino);
   if(!ino)
   {
	printf("Directory %s does not exist\n", argv[2]);
	return 0;
   }
   
   //get_block(fd, iblock, buf);

   printblock(dp);
}
printblock(DIR *DP)
{
   int ino, iblock, a, b;
   char *buf[BLKSIZE];
   char *ibf[BLKSIZE];

   get_block(fd, 2, buf);
   gp = (GD *)buf;

   ino = dp->inode;
   iblock = gp->bg_inode_table;
   a = (ino-1)/8 + iblock;
   b = (ino-1)%8;

   get_block(fd, a, ibf);
   ip = (INODE *)ibf+b;

   if(debug){
	inodedata(ip);

	//printf("ip->i_mode=%hu\n", ip->i_mode);
	}
   if(S_ISDIR(ip->i_mode))
	   dirnode(ip);
   else
   {
	int dirblk;		// direct block index
	int indirblk;		// indirect block index
	int inindirblk;		// double indirect block index

	int limit;		// limit for direct access loop
	int inlimit;		// limit for indirect access loop
	int ininlimit;		// limit for double indirect access loop

	int *indir;		// array at indirect block
	int *inindir;		// array at double indirect block
	int *ininindir;		// array at triple indirect block
	char dirbuf[BLKSIZE];

	int blocks = ip->i_blocks/2;
	char printbuf[BLKSIZE];


	if(debug){
	printf("======================direct blocks==================\n");
	getchar();}
	// if there's more than 12 blocks, put a hard cap on our loop
		// move on to the indirect block after
	limit = blocks;
	if(blocks > 11)
		limit = 12;
	for(dirblk = 0; dirblk < limit; dirblk++)
	{
	   get_block(fd, ip->i_block[dirblk], buf);
	   strcpy(printbuf, buf);
	   printf(printbuf);
	   blocks--;
	}
	if(debug){
	printf("====================indirect blocks==================\n");
	getchar();}
	// if there's more blocks than 256, put a hard cap on our loop
		// move on to the double indirect block after
	limit = blocks;
	if(blocks > 255)
		limit = 256;
	get_block(fd, ip->i_block[12], dirbuf);
	indir = (int *)dirbuf;
	for(dirblk = 0; indirblk < limit; indirblk++)
	{
	   get_block(fd, indir[dirblk], buf);
	   strcpy(printbuf, buf);
	   printf(printbuf);
	   blocks--;
	}
	// if there's more blocks than 256*256, put a hard cap on our loop
		// move on to the triple indirect block after
	if(debug){
	printf("=============double indirect blocks==================\n");
	getchar();}
	inlimit = (blocks+1)/256;
	if(blocks > 0 && inlimit == 0)
		inlimit = 1;
	if(blocks > (256*256)-1)
		inlimit = 256;
	get_block(fd, ip->i_block[13], dirbuf);
	inindir = (int *)dirbuf;
	for(indirblk = 0; indirblk < inlimit; indirblk++)
	{
	   // if there's more blocks than 256, put a hard cap on our loop
		// more on to the next indirect block after
	   limit = blocks;
	   if(blocks > 255)
		limit = 256;
	   get_block(fd, inindir[indirblk], dirbuf);
	   indir = (int *)dirbuf;
	   for(dirblk = 0; dirblk < limit; dirblk++)
	   {
		get_block(fd, indir[dirblk], buf);
		strcpy(printbuf, buf);
		printf(printbuf);
		blocks--;
	   }
	}
	if(debug){
	printf("=============triple indirect blocks==================\n");
	getchar();}
	// etc
	ininlimit = ((blocks+1)/256)/256;
	if(blocks > 0 && ininlimit == 0)
		ininlimit = 1;
	get_block(fd, ip->i_block[14], dirbuf);
	ininindir = (int *)dirbuf;
	for(inindirblk = 0; inindirblk < 256; inindirblk++)
	{
	   inlimit = (blocks+1)/256;
	   if(blocks > 0 && inlimit == 0)
		inlimit = 1;
	   if(blocks > (256*256)-1)
		inlimit = 256;
	   get_block(fd, ininindir[inindirblk], dirbuf);
	   inindir = (int *)dirbuf;
	   for(indirblk = 0; indirblk < inlimit; indirblk++)
	   {
		limit = blocks;
		if(blocks > 255)
		   limit = 256;
		get_block(fd, inindir[indirblk], dirbuf);
		indir = (int *)dirbuf;
		for(dirblk = 0; dirblk < 256; dirblk++)
		{
		   get_block(fd, indir[dirblk], buf);
		   strcpy(printbuf, buf);
		   printf(printbuf);
		   blocks--;
		}
	   }
	}

	//get_block(fd, ip->i_block[0], buf);
	//printf("%s\n", buf);
	/*
	int i;
	for(i=0;i<ip->i_size;i++)
	{
	   //if(debug)
		//putchar(i+48);
	   putchar(buf[i]);
	}
	*/

   }
}
printinodes()
{
	char *buf[BLKSIZE];
	int iblock = gp->bg_inode_table;
	get_block(fd, iblock, buf);
	int i = 0;
	for(i=1;i<35;i++)
	{
		ip = (INODE *)buf + i;
		inodedata(ip);
	}
}
