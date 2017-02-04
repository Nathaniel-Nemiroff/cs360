// search through the dir to find dir or file

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ext2type.h"

#define BLKSIZE 1024

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
int curblock;

// returns an empty DIR
DIR *emptydir()
{
   DIR *empt;
   empt->inode = 0;
   empt->rec_len = 0;
   empt->name_len = 5;
   empt->file_type = 1;
   strcpy(empt->name, "EMPTY");
   return empt;
}
// searches for the name in the individual directory
DIR *searchdir(INODE *ip, char *name)//, char dbuf[BLKSIZE])
{
   char ibuf[BLKSIZE];
   char dbuf[BLKSIZE];
   curblock = 0;

   // read GD
   //get_block(fd, 2, ibuf);
   //gp = (GD *)ibuf;

   //iblock = gp->bg_inode_table;
   //printf("inode_block=%d\n", iblock);

   // get first dir inode
   //get_block(fd, iblock, ibuf);
   //ip = (INODE *)ibuf + 1;// ip points at / (2nd inode)

   dblock = ip->i_block[curblock];
   get_block(fd, dblock, dbuf);
   dp = (DIR *)dbuf;
   char *cp = dp;

   int remaining = ip->i_size;

   while(remaining > 0)
   {
	if(debug){
	   printf("||%d", dp->inode);
	   printf("|%d", dp->rec_len);
	   printf("|%d", dp->name_len);
	   printf("|%d", dp->file_type);
	   printf("|name= \"");

	   int c;
	   for(c = 0; c < dp->name_len; c++)
	      putchar(dp->name[c]);
	   printf("\" ...\n");
	}

	char *dirname[dp->name_len];
	strncpy(dirname, dp->name, dp->name_len);

	//if(debug){
	//   printf("comparing %s to name); %s ...\n\n", name, dirname);
	if(!strncmp(name, dirname, dp->name_len)){
	   //if(dp->file_type == 1)
		//return 0;
	   if(debug)
	      printf("found at inode %d!\n", dp->inode);
	   return dp;
	}


	remaining -= dp->rec_len;

	/*
	int nextblock = (ip->i_size-remaining)/BLKSIZE;
	if(nextblock != curblock){
	   curblock = nextblock;
	   dblock = ip->i_block[curblock];
	   get_block(fd, dblock, dbuf);
	   dp = (DIR *)dbuf;
	   char *cp = dp;
	}
	*/

	cp += dp->rec_len;
	dp = (DIR *)cp;

	// add in support for inodes with more than one block
   }
   return emptydir;
}

// searches for the name in the entire disk
DIR *search(INODE *ip, char *name)
{
   char *buf[BLKSIZE];

   get_block(fd, 2, buf);
   gp = (GD *)buf;

   int imap = gp->bg_inode_bitmap;
   iblock = gp->bg_inode_table;
   get_block(fd, iblock, buf);

   int ino = 0;
   char *nametok;
   nametok = strtok(name, "/");

   while(nametok != NULL)
   {
	// deliminate name to get first item
	if(debug){
	   if(!ino)
	      printf("inode %d\n", ino);
	   printf("nametok=%s\n", nametok);
	}
	dp = searchdir(ip, nametok);
	ino = dp->inode;

	if(!ino)
	   return emptydir;
	int a, b;
	a = (ino-1)/8 + iblock;
	b = (ino-1)%8;

	get_block(fd, a, buf);
	ip = (INODE *)buf+b;

	// deliminate
	nametok = strtok(NULL, "/");
   }
   return dp;
}
