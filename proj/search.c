
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include "ext2type.h"
#include "FStype.h"
//#include "mainfunc.c"
//#include "FStype.h"

#define BLKSIZE 1024

/*
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
*/

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

//extern int fd;
extern int debug;
extern struct MOUNT *mounttable[10];
int iblock;
int dblock;
int curblock;

/*char *mytok(char *source, char delim, int *pos){
	int len = strlen(source);
	if(*pos>=len)
		return NULL;
	char retval[len];
	strcpy(retval, "");
	int i;
	for(i=*pos;i<len;i++){
		if(source[i]==delim)
			break;
		strncat(retval, &source[i], 1);
	}
	int temp = *pos;
	temp += i + 1;
	*pos = temp;
	printf("%d\n", *pos);
	return retval;
}*/
int mytok(char *source, char delim, int strt){
	int len = strlen(source);
	int i;
	for(i=strt;i<len;i++){
		if(source[i]==delim){
			break;
		}
	}
	return i-strt;
}
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
DIR *searchdir(int fd, INODE *ip, char *name)//, char dbuf[BLKSIZE])
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
   if(debug)
	   printf("running on block=%d for ip->i_size=%d\n", dblock, remaining);

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

	//if(debug)
	//   printf("comparing %s to name %s ...\n\n", name, dirname);
	if(!strncmp(name, dirname, dp->name_len))
		if(strlen(name)==dp->name_len){
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
   DIR *empt = (DIR *)malloc(sizeof(DIR));
   empt->inode = 0;
   return empt;
}

// searches for the name in the entire disk
struct MINODE *searchloop(int fd, INODE *ip, char *name, int link)
{
	if(debug)
		printf("search for %s\n", name);
	struct MINODE *retval   = NULL;//(struct MINODE *)sizeof(struct MINODE);
	struct MINODE *pretval  = NULL;

	struct MINODE *sretval  = NULL;//(struct MINODE *)sizeof(struct MINODE);
	struct MINODE *ptretval = NULL;//(struct MINODE *)sizeof(struct MINODE);
   char *buf[BLKSIZE];
   INODE pip;//previous ip
   //DIR sdp;//symlink dp
   //DIR ptdp;//inode that is pointed to by sdp
   			//saves the last symlink we find
			//and dir the symlink points to
			// in case we need to return it
	int type;

   get_block(fd, 2, buf);
   gp = (GD *)buf;

   int imap = gp->bg_inode_bitmap;
   iblock = gp->bg_inode_table;
   //get_block(fd, iblock, buf);

   int ino = 0;
   char nametok[64];
   //char *saveptr;
   int tokin = 0;
   int tokln = 0;
   //saveptr=zero;

   //nametok = strtok_r(name, "/", &saveptr);
   tokln = mytok(name, '/', tokin);
   if(tokln==0)
	   tokln=mytok(name, '/', ++tokin);
   if(tokln>0)
	   strncpy(nametok, &name[tokin], tokln);
	nametok[tokln]=0;
   tokin+=(tokln+1);
   //printf("HERE I AM\n");

   while(tokln>0)//nametok != NULL)
   {
	// deliminate name to get first item
	if(debug){
	    printf("inode %d\n", ino);
	   printf("nametok=%s\n", nametok);
	}
	dp = searchdir(fd, ip, &nametok);
	ino = dp->inode;
	type = dp->file_type;
	if(debug)
		printf("ino=%d type=%d\n", ino, dp->file_type);

	if(!ino){
		iput(retval);
		iput(pretval);
		iput(sretval);
		iput(ptretval);
	   return NULL;
	}
	//int a, b;
	//a = (ino-1)/8 + iblock;
	//b = (ino-1)%8;

	//get_block(fd, a, buf);

	iput(pretval);
	pretval = icopy(retval);
	iput(retval);
	retval = iget(fd, ino);
	pip = *ip;
	ip = &retval->ip;//(INODE *)buf+b;
	int m = getMount(retval);

	if(m>0){
		if(mounttable[m]->dev!=fd){
			fd = mounttable[m]->dev;
			iput(retval);
			if(debug)
				printf("going to different disk\n");
			retval = iget(fd, 2);
			ip = &retval->ip;
		}
	}
	else if(!strcmp(nametok, "..")){
		int b = 0;//boolean
		if(pretval!=NULL){
			if(pretval->ino==2)
				b=1;
		}
		else if(running->cwd->ino==2)
			b=1;
			if(b){
				if(retval->dev!=root->dev){
					if(debug)
						printf("coming back from different disk\n");
					int m = devToMount(retval->dev);// mount index
					iput(retval);
					retval = iget(mounttable[m]->mounted_inode->dev, mounttable[m]->mounted_inode->ino);
					int rdev = retval->dev;
					INODE retp = retval->ip;
					iput(retval);
					char dotdot[2];
					strcpy(dotdot, "..");
					printf("loopback\n");
					printf("%s\n", name);
					retval = searchloop(rdev, &retp, dotdot, link);
					printf("%s %s\n", name, nametok);
					printf("retval->dev=%d\n", retval->dev);
				}
			}
	}
	if(type==7){
		if(debug){
			printf("Found symlink!\n");
			char rbuf[BLKSIZE];
			get_block(fd, pip.i_block[0], rbuf);
			DIR *depr = (DIR *)rbuf;//debug print
			printf("pip's ino=%d\n", depr->inode);

		}
		//sdp = *dp;
		iput(sretval);
		sretval = icopy(retval);

		//char rbuf[BLKSIZE];//root buf
		//get_block(fd, ((2-1)/8)+iblock, rbuf);
		//INODE *rip = (INODE *)rbuf+((2-1)%8);//root
		//char pr[60];
		//strncpy(pr, ip->i_block, 60);
		//printf("%s\n", pr);
		char rrbuf[BLKSIZE];
		get_block(fd, pip.i_block[0], rrbuf);
		DIR *rdp = (DIR *)rrbuf;
		//char *rcp = rdp;
		//rcp+=rdp->rec_len;
		//rdp = (DIR *)rcp;
		char pr[60];
		memcpy(pr, ip->i_block, 60);
		if(debug){
			printf("searching at ino=%d ", rdp->inode);
			printf("for pathname=%s\n", pr);
		}
		iput(retval);
		retval = searchloop(fd, &pip, pr, link);
		*ip=retval->ip;
		printf("block[0]=%d\n", ip->i_block[0]);
		//ptdp = *dp;

		iput(ptretval);
		ptretval = icopy(retval);
		printf("ptretval->ino=%d\n", ptretval->ino);

		if(debug)
			printf("dp->ino=%d\nptdp.ino=%d\n", retval->ino, ptretval->ino);
	}
	// deliminate
	//nametok = strtok_r(NULL, "/", &saveptr);
	//nametok = mytok(name, '/', saveptr);

   tokln = mytok(name, '/', tokin);
   if(tokln>0)
	   strncpy(nametok, &name[tokin], tokln);
	nametok[tokln]=0;
   tokin+=(tokln+1);
   }
	if(link){
		//if(ptdp.inode==dp->inode)
			//dp = &sdp;
			if(debug)
				printf("link\n");
			if(ptretval!=NULL)
				if(ptretval->ino==retval->ino){
					iput(retval);
					retval = icopy(sretval);
				}
	}
	if(debug)
		printf("returning ino=%d from search\n", retval->ino);
	iput(pretval);
	iput(sretval);
	iput(ptretval);
   return retval;
}
int search(int fd, INODE *ip, char *name)
{
	if(!strcmp(name, "/"))
		return 2;
	//DIR *retval = searchloop(fd, ip, name, 0);
	//if(retval->file_type==2)
	struct MINODE *retval = searchloop(fd, ip, name, 0);
	if(retval==NULL)
		return 0;
	int isdir = S_ISDIR(retval->ip.i_mode);
	int ino = retval->ino;
	iput(retval);

	if(isdir)
		return ino;
	else
		return 0;
}
int searchfile(int fd, INODE *ip, char *name)
{
	if(!strcmp(name, "/"))
		return 2;
	//DIR *retval = searchloop(fd, ip, name, 0);
	//if(retval->file_type==1)
	struct MINODE *retval = searchloop(fd, ip, name, 0);
	if(retval==NULL)
		return 0;
	int isreg = S_ISREG(retval->ip.i_mode);
	int ino = retval->ino;
	iput(retval);

	if(isreg)
		return ino;
	else
		return 0;
}
int searchlink(int fd, INODE *ip, char *name)
{
	if(!strcmp(name, "/"))
		return 2;
	//DIR *retval = searchloop(fd, ip, name, 1);
	struct MINODE *retval = searchloop(fd, ip, name, 1);
	if(retval==NULL)
		return 0;
	int ino = retval->ino;
	iput(retval);

	return ino;
}
