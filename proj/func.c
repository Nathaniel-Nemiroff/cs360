// inact all of the functions in main.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include "ext2type.h"
#include "mainfunc.c"
#include "func1.c"
//#include "main.c"

#define BLKSIZE 1024

//typedef struct ext2_group_desc  GD;
//typedef struct ext2_super_block SUPER;
//typedef struct ext2_inode       INODE;
//typedef struct ext2_dir_entry_2 DIR;



extern int fd;
//extern pathname[128], parameter[16];
extern struct MINODE *root;
extern struct PROC *running;

// LEVEL 1
menu(){
int level1 = 1;
int level2 = 1;
int level3 = 1;

if(!level1){
	printf("Not implemented yet\n");
	return;
}
	printf("Items implemented:\n");
	printf("\tmkdir\n");//finished
	printf("\tcd\n");//finished
	printf("\tpwd\n");//finished
	printf("\tls\n");//finished
	printf("\trmdir\n");//finished
	printf("\tcreat\n");//finished
	printf("\tlink\n");//finished
	printf("\tunlink\n");//finished
	printf("\tsymlink\n");//finished
	printf("\trm\n");//finished
	printf("\tchmod\n");//finished
	printf("\tchown\n");//finished
	printf("\tstat\n");//finished
	printf("\ttouch\n");//finished
if(level2){
	printf("\topen\n");
	printf("\tclose\n");
	printf("\tpfd\n");
	printf("\tlseek\n");
	//printf("\taccess\n");
	printf("\tread\n");
	printf("\twrite\n");
	printf("\tcat\n");
	printf("\tcp\n");
	printf("\tmv\n");
}
if(level3){
	printf("\tmount\n");
	printf("\tumount\n");
	//printf("\tcs\n");
	//printf("\tfork\n");
	//printf("\tps\n");
}
	//printf("\tsync\n");
	printf("\tquit\n");

}
make(int mkdir, char *path){
	if(!strlen(path)){
		printf("Usage: mk <name>\n");
		return;
	}
	char dnm[128];
	char bnm[64];
	strcpy(dnm, dirname(path));
	strcpy(bnm, basename(path));
	if(debug){
		printf("dnm=%s\n", dnm);
		printf("bnm=%s\n", bnm);
	}

	// FIRST FIND THE PATH
	int dev;
	int ino = ialloc(running->cwd->dev);
	int bno = balloc(running->cwd->dev);
	int pno;
	if(strlen(dnm)){
		pno = getino(running->cwd->dev, dnm);
		dev = getdev(running->cwd->dev, dnm);
		if(!pno){
			printf("directory not found\n");
			return;
		}
	}
	else{
		pno = running->cwd->ino;
		dev = running->cwd->dev;
	}

	if(debug)
		printf("dev=%d ino=%d bno=%d pno=%d\n", dev, ino, bno, pno);

if(mkdir){
	char buf[BLKSIZE];
	get_block(dev, bno, buf);
	DIR *dp = (DIR *)buf;
	char *cp = dp;

	dp->inode = ino;
	dp->rec_len = 12;
	dp->name_len = 1;
	dp->file_type = 2;
	dp->name[0]='.';

	if(debug){
		printf("writing to block...\n");
		printf("%d ", dp->inode);
		printf("%d ", dp->rec_len);
		printf("%d ", dp->name_len);
		printf("%d ", dp->file_type);
		int c;
		for(c=0;c<dp->name_len;c++)
			putchar(dp->name[c]);
		putchar('\n');
	}

	cp += 12;
	dp = (DIR *)cp;

	dp->inode = pno;
	dp->rec_len = BLKSIZE-12;
	dp->name_len = 2;
	dp->file_type = 2;
	dp->name[0]='.';
	dp->name[1]='.';

	if(debug){
		printf("writing to block...\n");
		printf("%d ", dp->inode);
		printf("%d ", dp->rec_len);
		printf("%d ", dp->name_len);
		printf("%d ", dp->file_type);
		int c;
		for(c=0;c<dp->name_len;c++)
			putchar(dp->name[c]);
		putchar('\n');
	}

	if(WR)
		put_block(dev, bno, buf);
}

	struct MINODE *mip = iget(dev, ino);
	INODE *ip = &mip->ip;

	if(mkdir)
		ip->i_mode = 0x41ED;
	else
		ip->i_mode = 0x81A4;
	ip->i_uid = running->uid;
	ip->i_gid = running->gid;
	if(mkdir)
		ip->i_size = BLKSIZE;
	else
		ip->i_size = 0;
	if(mkdir)
		ip->i_links_count = 2;
	else
		ip->i_links_count = 1;
	ip->i_atime=ip->i_ctime=ip->i_mtime=time(0L);
	ip->i_blocks = 2;
	//if(mkdir)
		//ip->i_blocks += 2;
	//if(mkdir)
		ip->i_block[0]=bno;
	//else
		//ip->i_block[0]=0;
	int i;
	for(i=1;i<15;i++)
		ip->i_block[i]=0;
	mip->dirty = 1;

	if(WR)
		iput(mip);

	struct MINODE *pmip = iget(dev, pno);
	INODE *pip = &pmip->ip;

	if(mkdir)
		pip->i_links_count++;
	pip->i_atime=time(0L);
	pmip->dirty = 1;

	char pbuf[BLKSIZE];
	get_block(dev, pip->i_block[0], pbuf);
	DIR *pdp = (DIR *)pbuf;
	char *pcp = pdp;
	int reclen=0;

	int remaining = pip->i_size;
	while(remaining>0){

	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}
		reclen = pdp->rec_len;
		pcp += pdp->rec_len;
		pdp = (DIR *)pcp;
		remaining -= reclen;
	}
	pcp -= reclen;
	pdp = (DIR *)pcp;
	int ideal_len = 4*((8+pdp->name_len+3)/4);
	pdp->rec_len = ideal_len;
	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}
	pcp += pdp->rec_len;
	pdp = (DIR *)pcp;
	pdp->inode = ino;
	pdp->rec_len = reclen-ideal_len;
	pdp->name_len = strlen(bnm);
	if(mkdir)
		pdp->file_type = 2;
	else
		pdp->file_type = 1;
	for(i=0;i<pdp->name_len;i++)
		pdp->name[i]=bnm[i];

	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}
	if(WR)
		iput(pmip);

	if(WR)
		put_block(dev, pip->i_block[0], pbuf);
	// allocate space for new dir
		// inode and block
	// fill out the alloctated block
		// . and ..
	// put a record in parent dir inode
	// don't forget to update the link counts
}
make_dir(){
	make(1, pathname);
}
change_dir(){
//printf("Not implemented yet\n");
	int ino;
	int dev;
	INODE ip;
	if(strcmp(pathname, "")){
		/*if(pathname[0] == '/'){
			dev = root->dev;
			ip = root->ip;
		}
		else{
			dev = running->cwd->dev;
			//ip = running->cwd->ip;//iget(dev, running->cwd->ino)->ip;
		}*/
		//getchar();
		ino = getino(running->cwd->dev, pathname);
		if(debug)
			printf("============================================\n");
		dev = getdev(running->cwd->dev, pathname);
		//ino = search(dev, &ip, pathname);
		if(!ino){
			printf("directory not found\n");
			return;
		}
		if(debug)
			printf("ino=%d\n", ino);
		if(ino){
			iput(running->cwd);
			running->cwd = iget(dev, ino);
		}
		else
			printf("No pathname %s\n", pathname);
	}
	else{
		dev = root->dev;
		iput(running->cwd);
		running->cwd = iget(dev, 2);
	}
	//printf("running->i_block[0]=%d\n", running->cwd->ip.i_block[0]);
	//printf("after cd func\n\tdev=%d ino=%d\n", running->cwd->dev, running->cwd->ino);
   //if(no pathname)
	//cd to root
   // else
	//cd to pathname
		// search for pathname in chosen dir
		// change running proc's cwd to search result
}
pwdrec(INODE *ip, int ino, int dev){
	if(!S_ISDIR(ip->i_mode))
		return "FILE";
	if(ino==2){
		// MOUNT FIX
		if(0)
			printf("at root of %d\n", dev);
		if(dev!=root->dev){
			struct MINODE *mip;// = iget(dev, ino);
			int m = devToMount(dev);
			mip = icopy(mounttable[m]->mounted_inode);
			INODE ip = mip->ip;
			int i = mip->ino;
			int d = mip->dev;
			iput(mip);
			return pwdrec(&ip, i, d);
		}
		return "";
	}
	char buf[BLKSIZE];
	get_block(dev, ip->i_block[0], buf);
	DIR *dp = (DIR *)buf;

	int thisino = dp->inode;

	char *cp = dp;
	cp += dp->rec_len;
	dp = (DIR *)cp;

	int parino = dp->inode;
	char child_name[64] = "";
	int cn_len=0;

	if(0){
		printf("child ino=%d\n", ino);
		printf("this ino=%d\n", thisino);
		printf("parent ino=%d\n\n", parino);
	}

	if(ino){
		int remaining = ip->i_size-dp->rec_len;
		while(remaining>0){
			if(0){
				printf("%d ", dp->inode);
				printf("%d ", dp->rec_len);
				printf("%d ", dp->name_len);
				printf("%d ", dp->file_type);
				int c;
				for(c=0;c<dp->name_len;c++)
					putchar(dp->name[c]);
				putchar('\n');
			}
			if(dp->inode==ino){
				char val[64];
				strncpy(val, dp->name, dp->name_len);
				strcpy(child_name, "/");
				strcat(child_name, val);
				cn_len = dp->name_len+1;
				break;
			}
			remaining -= dp->rec_len;
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
		if(0)
			putchar('\n');
	}
	int a, b;
	a = (parino-1)/8 + iblock;
	b = (parino-1)%8;

	get_block(fd, a, buf);
	INODE *i = (INODE *)buf+b;

	char retval[128];
	strcpy(retval, pwdrec(i, thisino, dev));
	if(0){
		printf("val to add= ");
		int c;
		for(c=0;c<dp->name_len;c++)
			putchar(dp->name[c]);
		printf("\n%s\n", retval);
	}
	strncat(retval, child_name, cn_len);
	if(0)
		printf("combined=\"%s\"\n", retval);
	return retval;
}
pwd(){
//printf("Not implemented yet\n");
	if(debug){
		printf("finding path of ino=%d\n", running->cwd->ino);
		printf("i_block[0]=%d\n", running->cwd->ip.i_block[0]);
	}
	char pval[128];
	strcpy(pval, pwdrec(&running->cwd->ip, 0, running->cwd->dev));
	if(!strlen(pval))
		strcpy(pval, "/");
	printf("path:%s\n", pval);
	// recursive function
		// find parent inode in cwd's block
			// keep going until you reach root (inode = 2)
}
list_dir(){
   // int ino, dev = running->cwd->dev;
   // MINODE *mip = running->cwd;

	int dev;
	int ino;
	struct MINODE *mip;
	if(strcmp(pathname, "")){
		if(debug)
			printf("finding path...\n");
		/*if(pathname[0] == '/')
	   		dev = root->dev;
		else
			dev = running->cwd->dev;*/
		dev = getdev(running->cwd->dev, pathname);
		if(debug){
			printf("working in dev=%d and ", dev);}
		ino = getino(running->cwd->dev, pathname);
		if(debug)
			printf("ino=%d\n", ino);
		mip = iget(dev, ino);
	}
	else{
		if(debug)
			printf("getting this dir's info...\n");
		dev = running->cwd->dev;
		if(debug)
			printf("working in dev=%d and ", dev);
		ino = running->cwd->ino;
		if(debug)
			printf("ino=%d\n", ino);
		mip = icopy(running->cwd);//iget(dev, ino);
	}
	INODE IP = mip->ip;
	iput(mip);

	char buf[BLKSIZE];
	int blk = IP.i_block[0];
	get_block(dev, blk, buf);
	DIR *dp = (DIR *)buf;
	char *cp = dp;

	int remaining = IP.i_size;
	if(debug){
		printf("inode size=%d\n", remaining);
		printf("inode block=%d\n", IP.i_block[0]);
	}
	while(remaining>0){
		//print stuff
		struct MINODE *pmip = iget(dev, dp->inode);
		INODE pip = pmip->ip;//iget(dev, dp->inode)->ip;//print ip
		iput(pmip);

		mystat(pip.i_mode);
		printf(" %6d ",pip.i_size);
		int c;
		//for(c=0;c<dp->name_len;c++)
		for(c=0;c<16;c++)
			if(c+dp->name_len<16)
				putchar(' ');
			else
				putchar(dp->name[c+dp->name_len-16]);
		printf(" %s", ctime(&pip.i_ctime));

		remaining -= dp->rec_len;
		cp += dp->rec_len;
		dp = (DIR *)cp;
	}
   //mip points at minode
   // each data block of mip->INODE contains DIR entries
   // print the name strings of the DIR entries
}
rm(int rmdir, char path[128]){
	if(!strlen(path)){
		printf("Usage: rm <dir/file>\n");
	}
//printf("Not implemented yet\n");
	if(isBusy(path)){
		printf("Directory is busy\n");
		return;
	}
	char dnm[128];
	char bnm[64];
	strcpy(dnm, dirname(path));
	strcpy(bnm, basename(path));
	if(debug){
		printf("dnm=%s\n", dnm);
		printf("bnm=%s\n", bnm);
	}

	// FIRST FIND THE PATH
	int dev;
	int ino;
	int bno;
	int pno;
	if(strlen(dnm)){
		//INODE start = running->cwd->ip;
		//if(dnm[0]=='/')
		//	start = root->ip;
		//pno = search(dev, &start, dnm);
		pno = getino(running->cwd->dev, dnm);
		dev = getdev(running->cwd->dev, dnm);
		if(!pno){
			printf("directory not found\n");
			return;
		}
	}
	else{
		pno = running->cwd->ino;
		dev = running->cwd->dev;
	}

	struct MINODE *pmip = iget(dev, pno);
	INODE *pip = &pmip->ip;

	if(rmdir==1)
		ino = search(dev, pip, bnm);
	else if(rmdir==2)
		ino = searchlink(dev, pip, bnm);
	else
		ino = searchfile(dev, pip, bnm);
	if(!ino){
		printf("file not found\n");
		return;
	}

	struct MINODE *rmip = iget(dev, ino);
	INODE *rip = &rmip->ip;
	iput(rmip);

	bno = rip->i_block[0];
	char linkcheckbuf[BLKSIZE];
	get_block(dev, bno, linkcheckbuf);
	DIR *rdp = (DIR *)linkcheckbuf;

	if(!(rip->i_mode&S_IWOTH)){
		if(rip->i_uid!=running->uid&&rip->i_gid!=running->gid){
			if(debug)
				printf("*ip->uid=%d %d\n", rip->i_uid, running->uid);
			printf("You do not have permission to edit this file\n");
			return;
		}
		if(!((rip->i_uid==running->uid&&(rip->i_mode&S_IWUSR))||
		   (rip->i_gid==running->gid&&(rip->i_mode&S_IWGRP)))){
			if(debug)
				printf("ip->uid=%d %d\n", rip->i_uid, running->uid);
			printf("You do not have permission to edit this file\n");
			return;
		}
	}
	if(rmdir==1)
		if(rdp->file_type != 2){
			printf("Usage rmdir dirname\n");
			iput(pmip);
			return;
		}
	if(rmdir==2)
		if(rip->i_links_count == 1){
			if(rdp->file_type == 1)
				rmdir = 0;
			else
				rmdir = 1;

		}

	if(rmdir==1)
		pip->i_links_count--;
	if(WR)
		iput(pmip);

	if(debug)
		printf("ino=%d bno=%d\n", ino, bno);
	if(rmdir!=2)
		dialloc(dev, ino);
	if(rmdir==1)
		dballoc(dev, bno);
	else if(rmdir!=2){
		if(debug)
			printf("starting file deallocation\n");
		int blocks = rip->i_blocks/2;
		int i,j,k,l,m;
		char buf[BLKSIZE];
		char ibuf[BLKSIZE];
		char iibuf[BLKSIZE];
		for(i=0;i<blocks;i++){
			if(debug)
				printf("i=%d\n", i);
			j = i;
			if(i>65803){
				m = 14;
				l = ((i-65803)/256)/256;
				k = ((i-65803)/256)%256;
				j = (i-65803)%256;
				get_block(dev, rip->i_block[m], iibuf);
				if(debug)
					printf("gettting block[%d]...", k);
				int *ininindir = (int *)iibuf;
				if(debug)
					printf("sucessful!\n");
				get_block(dev, ininindir[l], ibuf);
				if(debug)
					printf("gettting block[%d]...", k);
				int *inindir = (int *)ibuf;
				if(debug)
					printf("successful!\n");
				get_block(dev, inindir[k], buf);
				if(debug);
					printf("gettting block[%d]...", k);
				int *indir = (int *)buf;
				if(debug)
					printf("successful!\n");

				dballoc(dev, indir[j]);
				if(debug)
					printf("successfully deallocated block %d\n", i);
				continue;
			}
			if(i>267){
				l = 13;
				k = (i-267)/256;
				j = (i-267)%256;
				get_block(dev, rip->i_block[l], ibuf);
				if(debug)
					printf("gettting block[%d]...", l);
				int *inindir = (int *)ibuf;
				if(debug)
					printf("sucessful!\n");
				get_block(dev, inindir[k], buf);
				if(debug)
					printf("gettting block[[%d]]...", k);
				int *indir = (int *)buf;
				if(debug)
					printf("successful!\n");
				dballoc(dev, indir[j]);
				if(debug)
					printf("successfully deallocated block %d\n", i);
				continue;
			}
			if(i>12){
				k = 12;
				j = (i-12);
				get_block(dev, rip->i_block[k], buf);
				if(debug)
					printf("gettting block[%d]...", k);
				int *indir = (int *)buf;
				if(debug)
					printf("success!\n");
				dballoc(dev, indir[j]);
				if(debug)
					printf("successfully deallocated block %d(%d, %d)\n", i, k, j);
				continue;
			}
			dballoc(dev, rip->i_block[j]);
				if(debug)
					printf("successfully deallocated block %d\n", i);
		}
	}

	char buf[BLKSIZE];
	get_block(dev, pip->i_block[0], buf);
	DIR *pdp = (DIR *)buf;
	char *cp = pdp;
	int rl1=pdp->rec_len;
	cp+=rl1;
	pdp=(DIR *)cp;
	//int rl2=pdp->rec_len;
	//cp+=rl2;
	//pdp=(DIR *)cp;

	int remaining = pip->i_size - (rl1);
	int reclen = 0;
	if(debug)
		printf("entering main dir removal loop\n");
	while(remaining>0){
		if(debug){
			printf("%d ", pdp->inode);
			printf("%d ", pdp->rec_len);
			printf("%d ", pdp->name_len);
			printf("%d ", pdp->file_type);
			int c;
			for(c=0;c<pdp->name_len;c++)
				putchar(pdp->name[c]);
			putchar('\n');
		}
		if(pdp->inode == ino){
			if(remaining==pdp->rec_len){
				if(debug)
					printf("Entry found at end of block!\n");
				cp-=reclen;
				pdp=(DIR *)cp;
				if(debug)
					printf("pdp->inode=%d\n", pdp->inode);
				pdp->rec_len+=remaining;
				if(debug){
					printf("result of removal:\n");
					printf("%d ", pdp->inode);
					printf("%d ", pdp->rec_len);
					printf("%d ", pdp->name_len);
					printf("%d ", pdp->file_type);
					int c;
					for(c=0;c<pdp->name_len;c++)
						putchar(pdp->name[c]);
					putchar('\n');
				}
			}
			else{
				int leftover = pdp->rec_len;
				memcpy(cp, cp+pdp->rec_len, remaining);
				remaining-=leftover;
				pdp=(DIR *)cp;
				while(remaining>0){
					if(debug){
						//printf("Entry found in middle of block!\n");
						printf("%d ", pdp->inode);
						printf("%d ", pdp->rec_len);
						printf("%d ", pdp->name_len);
						printf("%d ", pdp->file_type);
						int c;
						for(c=0;c<pdp->name_len;c++)
							putchar(pdp->name[c]);
						putchar('\n');
						printf("remainder=%d\n", remaining);
					}
					reclen = pdp->rec_len;
					cp+=pdp->rec_len;
					pdp=(DIR *)cp;
					remaining-=reclen;
				}
				cp-=reclen;
				pdp=(DIR *)cp;
				pdp->rec_len+=leftover;
			}
			break;
		}
		reclen = pdp->rec_len;
		cp+=reclen;
		pdp=(DIR *)cp;
		remaining-=reclen;
	}

	if(WR)
		put_block(dev, pip->i_block[0], buf);


	// if rmdir isn't empty
		// "NOT EMPTY"
	// else
		// deallocate inode and its block
		// remove the dir's record from parent's block
}
rmdir(){
	rm(1, pathname);
}
creat_file(){
//printf("Not implemented yet\n");
	make(0, pathname);
	// similar to mkdir
	// allocate inode and block
	// put record in parent dir
		// make sure it's noted as not a dir
}
ln(int type, char path[128], char param[128]){
	//add options for different link types and reversal
	if(!strcmp(param, "")){
		if(type==1){
			printf("Usage: link sourcedir linkname\n");
			return;
		}
		if(type==2){
			printf("Usage: symlink sourcefile linkname\n");
			return;
		}
	}
	char dnm[128];
	char bnm[64];
	char ndm[128];
	char nbm[64];
	strcpy(dnm, dirname(path));
	strcpy(bnm, basename(path));
	strcpy(ndm, dirname(param));
	strcpy(nbm, basename(param));
	if(debug){
		printf("dnm=%s\n", dnm);
		printf("bnm=%s\n", bnm);
		printf("ndm=%s\n", ndm);
		printf("nbm=%s\n", nbm);
	}

	// FIRST FIND THE PATH
	int dev;
	int ino;// = ialloc(dev);
	int idev;
	//int bno = balloc(dev);
	int pno;
	if(strlen(dnm)){
		//INODE start = running->cwd->ip;
		//if(dnm[0]=='/')
		//	start = root->ip;
		//pno = search(dev, &start, dnm);
		pno = getino(running->cwd->dev, dnm);
		dev = getdev(running->cwd->dev, dnm);
		//if(!pno){
			//printf("directory not found\n");
			//return;
		//}
	}
	else{
		pno = running->cwd->ino;
		dev = running->cwd->dev;
	}
	struct MINODE *smip = iget(dev, pno);
	INODE sip = smip->ip;
	iput(smip);
	if(type!=2){
		if(debug)
			printf("getting link's ino...\n");
		ino = searchfile(dev, &sip, bnm);
		idev = getdev(running->cwd->dev, param);

		if(debug)
			printf("link's ino = %d\n", ino);
		if(!ino){
			printf("file not available\n");
			return;
		}
	}
	else{
		if(debug)
			printf("getting symlink's ino...\n");
		ino = ialloc(dev);
		idev = dev;
		if(debug)
			printf("symlink's ino = %d\n", ino);
		if(ino==-1){
			printf("no more available inodes!\n");
			return;
		}
	}
	//struct MINODE *lmip = iget(dev, ino);

	if(debug)
		printf("dev=%d ino=%d pno=%d\n", dev, ino, pno);

	struct MINODE *mip = iget(idev, ino);
	INODE *ip = &mip->ip;

	struct MINODE *pmip = iget(dev, pno);
	INODE *pip = &pmip->ip;

	ip->i_links_count++;
	ip->i_atime=ip->i_ctime=ip->i_mtime=time(0L);
	//if(0)
	//	ip.i_mode = 0x41ED;
	//else
	//	ip.i_mode = 0x81A4;
	if(type==2){
		ip->i_mode=0120000;
		ip->i_uid = running->uid;
		ip->i_gid = running->gid;
	//if(0)
	//	ip.i_size = BLKSIZE;
	//else
		ip->i_size = 0;
		ip->i_links_count++;
		ip->i_blocks=0;

		char pr[60];
		//strcpy(pr, pwdrec(pip, 0, dev));
		//strcat(pr, "/");
		strcpy(pr, path);
		printf("pathname=%s\n", path);
		printf("pr=%s\n", pr);
	    memcpy(ip->i_block, pr, 60);//pwdrec(ip, 0, dev), 60);
		char ppr[60];
		memcpy(ppr, ip->i_block, 60);
		printf("ppr=%s\n", ppr);
	}
	//if(0)
	//	ip.i_links_count = 2;
	//else

	//ip.i_blocks = 2;
	//if(mkdir)
		//ip->i_blocks += 2;
	//if(mkdir)
	//	ip->i_block[0]=0;
	//else
		//ip->i_block[0]=0;
	//int i;
	//for(i=1;i<15;i++)
	//	ip->i_block[i]=0;
	mip->dirty = 1;

	if(WR)
		iput(mip);


	//if(0)
	//	pip->i_links_count++;
	pip->i_atime=time(0L);
	pmip->dirty = 1;

	char pbuf[BLKSIZE];
	get_block(dev, pip->i_block[0], pbuf);
	DIR *pdp = (DIR *)pbuf;
	char *pcp = pdp;
	int reclen=0;

	int remaining = pip->i_size;
	while(remaining>0){

	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}
		reclen = pdp->rec_len;
		pcp += pdp->rec_len;
		pdp = (DIR *)pcp;
		remaining -= reclen;
	}
	pcp -= reclen;
	pdp = (DIR *)pcp;
	int ideal_len = 4*((8+pdp->name_len+3)/4);
	pdp->rec_len = ideal_len;
	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}
	pcp += pdp->rec_len;
	pdp = (DIR *)pcp;
	pdp->inode = ino;
	pdp->rec_len = reclen-ideal_len;
	pdp->name_len = strlen(nbm);
	if(type==2){
		pdp->file_type = 7;
	}
	else
		pdp->file_type = 1;
	int i;
	for(i=0;i<pdp->name_len;i++)
		pdp->name[i]=nbm[i];
	if(debug){
		printf("%d ", pdp->inode);
		printf("%d ", pdp->rec_len);
		printf("%d ", pdp->name_len);
		printf("%d ", pdp->file_type);
		int c;
		for(c=0;c<pdp->name_len;c++)
			putchar(pdp->name[c]);
		putchar('\n');
	}

	if(WR)
		iput(pmip);

	if(WR)
		put_block(dev, pip->i_block[0], pbuf);

	// similar to creat
	// ONLY put record in parent dir
		// I THINK
}
link(){
	ln(1, pathname, parameter);
}
unlink(){
//printf("Not implemented yet\n");
	rm(2, pathname);
	// remove directory ONLY
		// I THINK
}
symlink(){
//printf("Not implemented yet\n");
	ln(2, pathname, parameter);
	// new inode ( and record )
		// put pathname in new inode, I think
}
rm_file(){
//printf("Not implemented yet\n");
	rm(0, pathname);
	// same as rmdir
	// deallocate inode and block
	// remove record from parent
}
ch_file(int chmod){
	if(!strlen(pathname)){
		printf("Usage: ch <file> <param>\n");
		return;
	}
//printf("Not implemented yet\n");
	char dnm[128];
	char bnm[64];
	strcpy(dnm, dirname(pathname));
	strcpy(bnm, basename(pathname));
	if(debug){
		printf("dnm=%s\n", dnm);
		printf("bnm=%s\n", bnm);
	}

	// FIRST FIND THE PATH
	int dev;
	int ino;
	int pno;
	if(strlen(dnm)){
		//INODE start = running->cwd->ip;
		//if(dnm[0]=='/')
		//	start = root->ip;
		//pno = search(dev, &start, dnm);
		pno = getino(running->cwd->dev, dnm);
		dev = getdev(running->cwd->dev, dnm);
		if(!pno){
			//if(chmod!=2){
				printf("directory not found\n");
				return;
			//}
			//creat_file();
			//return;
		}
	}
	else{
		pno = running->cwd->ino;
		dev = running->cwd->dev;
	}
	struct MINODE *mip = iget(dev, pno);
	INODE *ip = &mip->ip;
	iput(mip);
	//ino = search(dev, ip, bnm);
	struct MINODE *smip = searchloop(dev, ip, bnm, 0);
	ino = smip->ino;
	iput(smip);
	//ino = getinof(dev, pathname);

	if(!ino){
		if(chmod==2){
		creat_file();
		return;
		}
		printf("File not found\n");
		return;
	}
	struct MINODE *modmp = iget(dev, ino);
	INODE *modip = &modmp->ip;
	if(chmod==3){
		if(strlen(parameter))
			modip->i_gid = parameter;
		else
			modip->i_gid = running->gid;
	}
	if(chmod==2)
		modip->i_atime = time(0L);
	if(chmod==1)
		modip->i_mode = atoi(parameter);
	else if(chmod ==0){
		if(strlen(parameter))
			modip->i_uid = parameter;
		else
			modip->i_uid = running->uid;
	}
	modmp->dirty = 1;
	if(WR)
		iput(modmp);
	// simply change the inode.mode
	// CONSIDER adding in +r, +w, etc
}
chmod_file(){
	ch_file(1);
}
chown_file(){
//printf("Not implemented yet\n");
	ch_file(0);
	// simply change inode.uid to running.uid
}
chgrp(){
	ch_file(3);
}
mystat(int mode){
	     if(S_ISDIR(mode))
		printf("d");
	else if(S_ISLNK(mode))
		printf("l");
	else
		printf("-");
	printf((mode & S_IRUSR)?"r":"-");
	printf((mode & S_IWUSR)?"w":"-");
	printf((mode & S_IXUSR)?"x":"-");
	printf((mode & S_IRGRP)?"r":"-");
	printf((mode & S_IWGRP)?"w":"-");
	printf((mode & S_IXGRP)?"x":"-");
	printf((mode & S_IROTH)?"r":"-");
	printf((mode & S_IWOTH)?"w":"-");
	printf((mode & S_IXOTH)?"x":"-");
}
stat_file(){
	char dnm[128];
	char bnm[64];
	strcpy(dnm, dirname(pathname));
	strcpy(bnm, basename(pathname));
	if(debug){
		printf("dnm=%s\n", dnm);
		printf("bnm=%s\n", bnm);
	}

	// FIRST FIND THE PATH
	int dev;
	int ino;
	int pno;
	if(strlen(dnm)){/*
		INODE start = running->cwd->ip;
		if(dnm[0]=='/')
			start = root->ip;
		pno = search(dev, &start, dnm);*/
		pno = getino(running->cwd->dev, dnm);
		dev = getdev(running->cwd->dev, dnm);
		if(!pno){
			printf("directory not found\n");
			return;
		}
	}
	else{
		pno = running->cwd->ino;
		dev = running->cwd->dev;
	}

	struct MINODE *pmip = iget(dev, pno);
	INODE *pip = &pmip->ip;
	iput(pmip);

	ino = search(dev, pip, bnm);
	struct MINODE *mip = iget(dev, ino);
	INODE *ip = &mip->ip;
	iput(mip);

	int mode = ip->i_mode;

	mystat(mode);
	printf("\n");
   // struct stat mystat; print mystat information
   // the inode.mode I think
}
touch_file(){
//printf("Not implemented yet\n");
	ch_file(2);
	// search for file
	// if file exists
		//change time to current
	//else
		// make new file
}

sync(){
printf("Not implemented yet\n");
	// ???
}
quit(){
//printf("Not implemented yet\n");
	// write all dirty inodes to disk
	// close all devices
	if(debug)
		printf("closing dev all files...\n");
	int i;
	int j;
	for(i=0;i<100;i++)
		for(j=0;j<10;j++){
			if(proc[i]->uid>-1)
				if(proc[i]->fd[j]->refCount>0)
					myclose(proc[i], j);}
	if(debug)
		printf("closing all mounted disks\n", running->cwd->dev);
	//close(running->cwd->dev);
	for(i=9;i>-1;i--)
		if(mounttable[i]->dev>0){
			if(i==0&&mounttable[0]->refCount==2)
				iput(running->cwd);
			if(close(mounttable[i]->dev)==-1){
				printf("close uncuccessful\n");
				return -1;
			}
		}
	if(debug)
		printf("close successful, exiting now...\n");
	exit(1);
// iput all DIRTY minodes before shutdown
}
