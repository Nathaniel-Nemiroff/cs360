// inact functions used in initialization

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include "ext2type.h"
#include "FStype.h"
//#include "func.c"
//#include "search.c"

#define BLKSIZE 1024

/*typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;*/

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

int fd;
int imap, bmap;
int iblock;
int ninodes, nblocks, nfreeInodes, nfreeBlocks;
int cwd;

extern int debug;
extern int WR;

/*
struct PROC{
   int pid, gid;
   int uid;
   int status;
   struct MINODE *cwd;
   struct OFT *fd[10];// files opened by this proc SET TO ZERO
};
struct MINODE{
   INODE ip;
   int dev;
   int ino;
   int refCount;
   int dirty;
   int mounted;
   struct MOUNT *mountptr;
};
struct OFT{
	int mode;
	int refCount;
	struct MINODE *mip;
	int offset;
};
struct MOUNT{
	int dev;
	int nblocks, ninodes;
	int bmap, imap, iblk;
	struct MINODE *mounted_inode;
	char name[64];
	char mount_name[64];
};

struct PROC *proc[100];
// PROC proc[100];
struct MINODE *minode[100];
struct MINODE *root;
struct PROC *running;
struct MOUNT *mounttable[10];
char pathname[128], parameter[60];
*/

// put these in another utility file
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
int tst_bit (char *buf, int bit)
{
   int i, j;
   i=bit/8; j=bit%8;
   if(buf[i]&(1<<j))
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

changeFreeNums(int dev, int mod, int type){//inc/dec free inodes/blocks
	char buf[BLKSIZE];					// will inc or dec based on mod and type
										// type=1: inc/dec inodes
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	if(type)
		sp->s_free_inodes_count+=mod;
	else
		sp->s_free_blocks_count+=mod;
	if(WR)
		put_block(dev, 1, buf);

	get_block(dev, 2, buf);
	gp = (GD *)buf;
	if(type)
		gp->bg_free_inodes_count+=mod;
	else
		gp->bg_free_blocks_count+=mod;
	if(WR)
		put_block(dev, 2, buf);
}
ialloc(int dev){
	int i;
	char buf[BLKSIZE];
	get_block(dev, imap, buf);
	for(i=0;i<ninodes;i++){
		if(tst_bit(buf,i)==0){
			set_bit(buf,i);
			changeFreeNums(dev, -1, 1);
			if(WR)
				put_block(dev, imap, buf);

			return i+1;
		}
	}
	printf("No more free inodes!\n");
	return 0;
}
dialloc(int dev, int ino){
	char buf[BLKSIZE];
	get_block(dev, imap, buf);
		//if(tst_bit(buf,ino-1)){
			clr_bit(buf, ino-1);
			if(WR){
				changeFreeNums(dev, 1, 1);
				put_block(dev, imap, buf);}
		//}
	//printf("Inode not used!\n");
	//return 0;
}
balloc(int dev){
	int i;
	char buf[BLKSIZE];
	get_block(dev, bmap, buf);
	for(i=0;i<nblocks;i++){
		if(tst_bit(buf, i)==0){
			set_bit(buf,i);
			changeFreeNums(dev, -1, 0);
			if(WR)
				put_block(dev, bmap, buf);
			return i+1;
		}
	}
	printf("No more free blocks!\n");
	return 0;
}
dballoc(int dev, int bno){
	char buf[BLKSIZE];
	get_block(dev, bmap, buf);
	//	if(tst_bit(buf,bno-1)){
			clr_bit(buf,bno-1);
			if(WR){
				changeFreeNums(dev, 1, 0);
				put_block(dev, bmap, buf);}
	//	}
	//printf("Block not allocated!\n");
	//return 0;
}

dirname(char *pathname){
	char data[128];
	strcpy(data, pathname);
	if(debug)
		printf("path=%s\n", pathname);
	char *tok;
	char retval[strlen(pathname)];
	char prevtok[64];
	strcpy(prevtok, "");
	tok = strtok(data, "/");
	if(pathname[0]=='/')
		strcpy(retval, "/");
	else
		strcpy(retval, "");
	while(tok != NULL)
	{
		strcat(retval, prevtok);
		if(strcmp(prevtok, ""))
			strcat(retval, "/");
		strcpy(prevtok, tok);
		tok = strtok(NULL, "/");
	}
	retval[strlen(retval)-1]=0;
	return retval;
}
basename(char *pathname){
	char data[128];
	strcpy(data, pathname);
	char *tok;
	char prevtok[64];
	tok = strtok(pathname, "/");
	while(tok!=NULL)
	{
		strcpy(prevtok, tok);
		tok = strtok(NULL, "/");
	}
	return prevtok;
}
int getinof(int dev, char *pathname){
	INODE inode;
	if(pathname[0]=='/')
		inode = root->ip;
	else
		inode = running->cwd->ip;

	int ret=searchfile(dev, &inode, pathname);
	//return searchfile(dev, &inode, pathname);
	return ret;
}
int getino(int dev, char *pathname)
{
   //printf("getino Not implemented yet\n");
   //char *buf[BLKSIZE];
   //get_block(dev, iblock, buf);
   INODE inode;
   	if(pathname[0]=='/')
		inode = root->ip;
	else
   		inode = running->cwd->ip;//(INODE *)buf+1;

   //DIR *DP = search(dev, inode, pathname);
   return search(dev, &inode, pathname);//DP->inode;
}
int getdev(int dev, char *path){
	if(!strcmp(path, "/"))
		return root->dev;
	INODE inode;
	if(path[0]=='/')
		inode = root->ip;
	else
		inode = running->cwd->ip;

	struct MINODE *retval = searchloop(dev, &inode, path, 0);
	if(retval==NULL)
		return 0;
	int ret = retval->dev;
	iput(retval);

	return ret;
}

int getFreeMinode(){
	int i;
	for(i=0; i<100; i++){
		if(minode[i]->refCount == 0)
			return i;
	}
	return -1;
}
struct MINODE *iget(dev, ino)
{
	//printf("iget Not implemented yet\n");
	int i;
	for(i=0;i<100;i++)
		if(minode[i]->ino == ino)
			if(minode[i]->dev == dev){
				minode[i]->refCount++;
				return minode[i];
			}
	i = getFreeMinode();
	//if(i==-1)
	//	NO MORE MEMORY
	// SEARCH MINODE[] FOR IT FIRST
   char buf[BLKSIZE];
   struct MINODE *mip;
   mip = (struct MINODE *)malloc(sizeof(struct MINODE));

   int a = (ino-1)/8 + iblock;
   int b = (ino-1)%8;

   get_block(dev, a, buf);
   INODE *IP = (INODE *)buf+b;
   //if(debug)
	//   printf("IP->i_size=%d\n", IP->i_size);

   int j = getFreeMinode();
   if(j < 0)
	   return NULL;

	mip->ip = *IP;
	mip->dev = dev;
	mip->ino = ino;
	mip->refCount = 1;
	mip->dirty = 0;
	//if(debug)
	//	printf("returning minode:\tdev=%d ino=%d\n", mip->dev, mip->ino);
	minode[i] = mip;
	return mip;
}
struct MINODE *icopy(struct MINODE *mip){
	if(mip == NULL){
		return NULL;
	}
	if(debug)
		printf("getting mip at %d %d\n", mip->dev, mip->ino);
	return iget(mip->dev, mip->ino);
}
iput(struct MINODE *mip){
	if(mip==NULL)
		return;
	if(mip->dirty){
	//mip->refCount--;

	char buf[BLKSIZE];
	int a = (mip->ino-1)/8 + iblock;
	int b = (mip->ino-1)%8;

	//char *bufcpy;
	//bufcpy = &mip->ip;
	get_block(mip->dev, a, buf);
	INODE *in = (INODE *)buf+b;
	*in = mip->ip;
	if(debug){
		INODE *test = (INODE *)buf+b;

		printf("written values:\n");
		printf("i_mode=%d\n", test->i_mode);
		printf("i_size=%d\n", test->i_size);
		printf("i_block[0]=%d\n", test->i_block[0]);
	}
	//strncpy(bufcpy, (char *)mip->ip, BLKSIZE);//change this
	//strncpy(buf, bufcpy, BLKSIZE);

	if(WR)
		put_block(mip->dev, a, buf); // TEST THIS
	mip->dirty=0;
	}
	mip->refCount--;
	if(mip->refCount<=0){
		int i;
		for(i=0;i<100;i++)
			if(minode[i]->ino == mip->ino)
				if(minode[i]->dev == mip->dev){
					if(0)
						printf("deallocating minode\n");
					minode[i]->ino=0;
					minode[i]->dev=0;
					// fix the mount table
					break;
				}
	}
}
int OFTRdRdy(struct MINODE *mip){
	int i;
	int j;
	if(debug)
		printf("running: %d\n", proc[3]->pid);
	for(i=0;i<100;i++){
		for(j=0;j<10;j++){
			if(proc[i]->uid>-1){
				if(0){
					printf("checking if proc[%d]->fd[%d] is free\n", i, j);
					printf("mip->ip = %d\n", mip->ino);
					printf("proc->cwd->ino=%d\n", proc[i]->cwd->ino);
					printf("fd->ino = %d\n", proc[i]->fd[j]->mip->ino);
				}
				if(proc[i]->fd[j]->mip->ino == mip->ino){
					if(proc[i]->fd[j]->mip->dev = mip->dev){
						if(proc[i]->fd[j]->mode>0){
							if(debug)
								printf("file not available for read!\n");
							return 0;
						}
					}
				}
			}
		}
	}
	if(debug)
		printf("file is ready for read!\n");
	return 1;
}
struct OFT *nullfd(){
	struct OFT *oftp = (struct OFT *)malloc(sizeof(struct OFT));
	oftp->mip = (struct MINODE *)malloc(sizeof(struct MINODE));
	oftp->mip->ino=0;
	oftp->refCount=0;
}
initProcFd(struct PROC *proc){
	int i;
	for(i=0;i<10;i++){
		proc->fd[i]=nullfd();
		//proc->fd[i]=(struct OFT *)malloc(sizeof(struct OFT));
		//proc->fd[i]->mip=(struct MINODE *)malloc(sizeof(struct MINODE));
		//proc->fd[i]->mip->ino=0;
		//proc->fd[i]->refCount=0;
	}
}
//char *disk = "mydisk";

init(char *disk){
// initialize data structures of LEVEL 1

// 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
// MINODE minode[100]; with all refCount=0
// MINODE *root = 0;
if(debug)
	printf("openning %s...\n", disk);
fd = open(disk, O_RDWR);// CHECK THE MAGIC NUMBER
if(debug)
	printf("result: fd=%d\n", fd);
if(fd < 0){
	printf("open failed\n");
	return -1;
}
if(debug)
	printf("Open succeeded!\n");

char buf[BLKSIZE];
get_block(fd, 2, buf);
gp = (GD *)buf;
char sbuf[BLKSIZE];
get_block(fd, 1, sbuf);
sp = (SUPER *)sbuf;

if(debug)
	printf("s_magic = %x\n", sp->s_magic);
if(sp->s_magic != 0xEF53){
	if(debug)
		printf("NOT an EXT2 FS\n");
	return -1;
}

bmap = gp->bg_block_bitmap;
imap = gp->bg_inode_bitmap;
iblock = gp->bg_inode_table;
ninodes = sp->s_inodes_count;
nblocks = sp->s_blocks_count;


int i;
for(i = 0; i < 100; i++){
	minode[i] = (struct MINODE *)malloc(sizeof(struct MINODE));
	minode[i]->refCount = 0;
}

root = (struct MINODE *)malloc(sizeof(struct MINODE));
root = iget(fd, 2);

for(i=0;i<100;i++){
	proc[i]=(struct PROC *)malloc(sizeof(struct PROC));
	proc[i]->uid=-1;
}

//proc[0] = (struct PROC *)malloc(sizeof(struct PROC));
proc[0]->uid = 0;
proc[0]->pid = 0;
proc[0]->cwd = root;
initProcFd(proc[0]);

//proc[1] = (struct PROC *)malloc(sizeof(struct PROC));
proc[1]->uid = 1;
proc[1]->pid = 1;
proc[1]->cwd = root;
initProcFd(proc[1]);

for(i=0;i<10;i++){
	mounttable[i]=(struct MOUNT *)malloc(sizeof(struct MOUNT));
	mounttable[i]->dev=0;
	mounttable[i]->refCount=0;
}

mounttable[0]->dev=fd;
mounttable[0]->bmap=bmap;
mounttable[0]->imap=imap;
mounttable[0]->iblk=iblock;
mounttable[0]->ninodes=ninodes;
mounttable[0]->nblocks=nblocks;

strcpy(mounttable[0]->name, disk);
mounttable[0]->mounted_inode=root;
root->mountptr=mounttable[0];

//minode[0]=iget(
//minode[0]->dev = fd;
//minode[0]->ino = 2;
//minode[0]->refCount = 1;
//minode[0]->dirty = 0;



// GET IMPORTANT NUMBERS LIKE IBLOCK

}
