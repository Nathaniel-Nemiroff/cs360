// LEVEL 2

#include "func2.c"

extern int WR;

#define BLKSIZE 1024

truncate(struct MINODE *mip){
	int dev = mip->dev;
	int blocks = mip->ip.i_blocks/2;
	int *i_blocks = (int *)mip->ip.i_block;
	int i, j, k, l, m;
	char buf[BLKSIZE];
	char ibuf[BLKSIZE];
	char iibuf[BLKSIZE];
	if(debug)
		printf("starting loop with %d blocks...\n", blocks);
	for(i=0;i<blocks;i++){
		if(debug)
			printf("dallocing block %d...", i);
		j=i;
		//printf("j=%d\n", j);
		if(i>65803){
			m=14;
			l=((i-65803)/256)/256;
			k=((i-65803)/256)%256;
			j=(i-65803)%256;
			get_block(dev,  i_blocks[m], iibuf);
			int *ininindir=(int *)iibuf;
			get_block(dev, ininindir[l],  ibuf);
			int   *inindir=(int *) ibuf;
			get_block(dev,   inindir[k],   buf);
			int     *indir=(int *)  buf;

			dballoc(dev, indir[j]);
			if(debug)
				printf("success!\n");
			continue;
		}
		if(i>267){
			l=13;
			k=(i-267)/256;
			j=(i-267)%256;
			get_block(dev,  i_blocks[l],  ibuf);
			int   *inindir=(int *) ibuf;
			get_block(dev,   inindir[k],   buf);
			int     *indir=(int *)  buf;

			dballoc(dev, indir[j]);
			if(debug)
				printf("success!\n");
			continue;
		}
		if(i>12){
			k=12;
			j=(i-12);
			get_block(dev, 	i_blocks[k],   buf);
			int     *indir=(int *)  buf;

			dballoc(dev, indir[j]);
			if(debug)
				printf("success!\n");
			continue;
		}
		//printf("j=%d\n", j);
		dballoc(dev, i_blocks[j]);
		if(debug)
			printf("success!\n");
	}

	if(debug)
		printf("complete\n");
	mip->ip.i_atime = mip->ip.i_mtime = time(0L);
	mip->ip.i_size = 0;
	int d;
	for(d=0;d<15;d++)
		mip->ip.i_block[d]=0;
	mip->dirty = 1;
	if(debug)
		printf("mip written to!\n");
}
int getMode(char *c){
	if(debug)
		printf("opening with mode %s\n", c);
	if(!strcmp(c, "0")||!strcmp(c, "R")||!strcmp(c, "r")){
		if(debug)
			printf("opening for read\n");
		return 0;
	}
	if(!strcmp(c, "1")||!strcmp(c, "W")||!strcmp(c, "w")){
		if(debug)
			printf("opening for write\n");
		return 1;
	}
	if(!strcmp(c, "2")||!strcmp(c, "RW")||!strcmp(c,"rw")){
		if(debug)
			printf("opening for read/write\n");
		return 2;
	}
	if(!strcmp(c, "3")||!strcmp(c, "APPEND")||!strcmp(c, "append")){
		if(debug)
			printf("opening for append\n");
		return 3;
	}
}

myopen(char *filename, int OPENMODE){
	if(!strlen(filename))
		return -1;
	//printf("Not implemented yet\n");

	// MIP = iget
	int dev;
	int ino;

	/*
	   INODE sip;// starting ip
	   if(filename[0]=='/'){
	   dev = root->dev;
	   sip = root->ip;
	   }
	   else{
	   dev = running->cwd->dev;
	   sip = running->cwd->ip;
	   }
	   ino = searchfile(dev, &sip, filename);
	 */
	dev = getdev(running->cwd->dev, filename);
	ino = getinof(running->cwd->dev, filename);

	if(!ino){
		printf("File not found\n");
		return -1;
	}

	struct MINODE *mip = iget(dev, ino);
	INODE pp=mip->ip;//permissions ip
	/*if(OPENMODE > 0){
	  if(!(pp.i_mode&S_IWOTH)){
	  if(pp.i_uid!=running->uid&&pp.i_gid!=running->gid){
	  if(debug)
	  printf("ip->uid=%d\n", pp.i_uid);
	  printf("You do not have permission to edit this file\n");
	  iput(mip);
	  return -1;
	  }
	  if(!(pp.i_uid==running->uid&&(pp.i_mode&S_IWUSR))||
	  !(pp.i_gid==running->gid&&(pp.i_mode&S_IWGRP))){
	  if(debug)
	  printf("ip->uid=%d\n", pp.i_uid);
	  printf("You do not have permission to edit this file\n");
	  iput(mip);
	  return -1;
	  }
	  }
	  }
	  else if(!OPENMODE){
	  if(!(pp.i_mode&S_IROTH)){
	  if(pp.i_uid!=running->uid&&pp.i_gid!=running->gid){
	  if(debug)
	  printf("ip->uid=%d\n", pp.i_uid);
	  printf("You do not have permission to edit this file\n");
	  iput(mip);
	  return -1;
	  }
	  if(!(pp.i_uid==running->uid&&(pp.i_mode&S_IRUSR))||
	  !(pp.i_gid==running->gid&&(pp.i_mode&S_IRGRP))){
	  if(debug)
	  printf("ip->uid=%d\n", pp.i_uid);
	  printf("You do not have permission to edit this file\n");
	  iput(mip);
	  return -1;
	  }
	  }
	  }*/
	if(debug){
		printf("preparing file at mip->dev=%d ->ino=%d", mip->dev, mip->ino);
		printf(" with mode=%d\n", OPENMODE);
	}
	// we know it's a file from searchfile
	// implement permission checking later
	// CHECK IF FILE IS OK FOR MODE
	if(!OFTRdRdy(mip)){
		iput(mip);
		return -1;
	}

	//int i = getFreeOFT();//i = oft index
	// ALLOC OFT
	struct OFT *oftp = (struct OFT *)malloc(sizeof(struct OFT));
	oftp->mode = OPENMODE;
	oftp->refCount = 1;
	oftp->mip = mip;

	if(debug)
		printf("setting write mode %d\n", OPENMODE);
	// SET OFFSET
	switch(OPENMODE){
		case 0: oftp->offset = 0; break;	// R: start from 0
		case 1: oftp->offset = 0;
				if(WR)
					truncate(mip);
				break;		// W: truncate file
		case 2: oftp->offset = 0;	break;	// RW: no truncation
		case 3: oftp->offset = mip->ip.i_size; // APPEND
				break;						// start at end
		default: printf("invalid mode\n");return -1;//should never happen
	}

	if(debug)
		printf("finding free oft\n");
	// PUT IT IN RUNNING PROC's FD (SMALLEST FD[FD])
	int j;
	for(j=0;j<10;j++)
		if(running->fd[j]->refCount==0){
			running->fd[j]=oftp;
			break;
		}
	if(debug)
		printf("found free oft at %d\n", j);
	// if j > fd.size, NO MEMORY

	// UPDATE INODE TIME
	if(!OPENMODE)
		mip->ip.i_atime = time(0L);
	else
		mip->ip.i_atime = mip->ip.i_mtime = time(0L);
	mip->dirty = 1;

	// RETURN WHERE YOU PUT IT IN RUNNING->FD
	return j;

	// assign file to running proc
	// create oft
	// unless already open
	// open file for reading and assign it to oft
}
open_file(){
	int fd = myopen(pathname, getMode(parameter));
	if(fd<0)
		printf("File can't be opened\n");
}
myclose(struct PROC *p, int fd){
	if(fd > 10||fd<0){
		printf("No file to close\n");
		return -1;
	}
	if(p->fd[fd]->refCount==0){
		printf("No file to close\n");
		return -1;
	}
	struct OFT *op = p->fd[fd];
	p->fd[fd] = nullfd();
	op->refCount--;
	if(op->refCount>0)
		return 0;
	struct MINODE *mip = op->mip;
	iput(mip);
	return 0;
}
int getNum(char *param){
	if(!strcmp(param, ""))
		return -1;
	int i;
	int j=0;
	for(i=0;i<strlen(param);i++){
		if(!isdigit(param[i]))
			return -1;
		j = j*10;
		j+=(param[i]-'0');
	}
	return j;
}
close_file(){
	//printf("Not implemented yet\n");

	// GET FD
	myclose(running, getNum(pathname));

	// delete file from running proc
	// if reference number isn't 0
	//close file
}
pfd(){
	//printf("Not implemented yet\n");
	printf(" fd     mode   offset   inode\n");
	int i;
	for(i=0;i<10;i++){
		struct OFT *oftp = running->fd[i];
		if(oftp->refCount>0){
			printf(" %2d   ", i);
			switch(oftp->mode){
				case 0: printf("     R"); break;
				case 1: printf("     W"); break;
				case 2: printf("    RW"); break;
				case 3: printf("APPEND"); break;
				default:printf(" ERROR");
			}
			printf(" %8d  [%d, %2d]\n", oftp->offset, oftp->mip->dev, oftp->mip->ino);
		}
	}

	// just go through all the fds and print them all
}
myseek(int fd, int position){
	if(fd<0||position<0){
		if(debug)
			printf("value not valid\n");
		return -1;
	}
	struct OFT *op = running->fd[fd];
	struct MINODE *mip = op->mip;
	int size = mip->ip.i_size;
	iput(mip);
	int retval = op->offset;

	if(position>size){
		printf("Cannot seek past file's size\n");
		return -1;
	}
	else{
		op->offset = position;
		return retval;
	}
}
lseek_file(){
	//printf("Not implemented yet\n");
	// just lseek on the file
	myseek(getNum(pathname), getNum(parameter));
}
access_file(){
	printf("Not implemented yet\n");
	// ??
}
myread(int fd, char rbuf[], int nbytes){
	printf("fd=%d, nbytes=%d ", fd, nbytes);
	if(running->fd[fd]->refCount>0)
		if(running->fd[fd]->mode!=0 && running->fd[fd]->mode!=2)
			return -1;

	struct OFT *oftp = running->fd[fd];
	int dev    = oftp->mip->dev;
	int ino    = oftp->mip->ino;
	int offset = oftp->offset;
	int lblk   = offset / BLKSIZE;
	int start  = offset % BLKSIZE;
	int avil   = oftp->mip->ip.i_size-offset;
	int bytesread = 0;

	if(nbytes<(BLKSIZE-start))
		bytesread=nbytes;
	else
		bytesread=BLKSIZE-start;
	if(bytesread>avil)
		bytesread=avil;

	int blk;

	// get datablock
	//int blk = oftp->mip->ip.i_block[lbk];

	if((oftp->mip->ip.i_blocks/2)<lblk)
		return 0;

	int *i_blocks = (int *)oftp->mip->ip.i_block;
	int  i, j, k, l, m;
	char buf[BLKSIZE];
	char ibuf[BLKSIZE];
	char iibuf[BLKSIZE];
	i=lblk;
	j=i;
	//printf("lblk=%d 1111111111111111111111111111111111111111111111\n", lblk);
	//printf("blocks=%d\n", oftp->mip->ip.i_blocks/2);
	if(i>65803){
		m=14;
		l=((i-65803)/256)/256;
		k=((i-65803)/256)%256;
		j=(i-65803)%256;
		get_block(dev,  i_blocks[m], iibuf);
		int *ininindir=(int *)iibuf;
		get_block(dev, ininindir[l],  ibuf);
		int   *inindir=(int *) ibuf;
		get_block(dev,   inindir[k],   buf);
		int     *indir=(int *)  buf;

		blk=indir[j];
	}
	else if(i>267){
		l=13;
		k=(i-267)/256;
		j=(i-267)%256;
		get_block(dev,  i_blocks[l],  ibuf);
		int   *inindir=(int *) ibuf;
		get_block(dev,   inindir[k],   buf);
		int     *indir=(int *)  buf;

		blk=indir[j];
	}
	else if(i>11){
		k=12;
		j=(i-12);
		get_block(dev, 	i_blocks[k],   buf);
		int     *indir=(int *)  buf;

		blk=indir[j];
	}
	else
		blk=i_blocks[j];
	printf("blk=%d\n", blk);

	char tbuf[BLKSIZE];//temptorary buf
	get_block(dev, blk, tbuf);

	strncpy(rbuf, &tbuf[start], bytesread);

	oftp->offset+=bytesread;
	return bytesread;
}
read_file(){
	int fd = getNum(pathname);
	int nbytes = getNum(parameter);
	int n;
	char buf[BLKSIZE];

	if(fd>10||fd<0)
		return;
	int c;
	while(n=myread(fd, buf, nbytes)){
		for(c=0;c<n;c++)
			putchar(buf[c]);
		nbytes-=n;
	}
	putchar('\n');
	// get_block?
}
writeblock(struct OFT *oftp){
	oftp->mip->ip.i_blocks+=2;
	oftp->mip->dirty=1;
	//iput(oftp->mip);
	int blk = balloc(oftp->mip->dev);
	if(blk < 0){
		printf("RAN OUT OF SPACE\n");
		exit(1);
	}
	printf("\nallocated block %d\n", blk);
	//getchar();
	char clr[BLKSIZE];
	int i;
	for(i=0;i<BLKSIZE;i++)
		clr[i]=0;
	put_block(oftp->mip->dev, blk, clr);
	return blk;
}
mywrite(int fd, char wbuf[], int nbytes){
	if(debug)
		printf("writing data to disk....\n");
	if(running->fd[fd]->refCount>0){
		if(running->fd[fd]->mode!=1 && running->fd[fd]->mode!=2&&
				running->fd[fd]->mode!=3)
			return -1;
	}
	else
		return -1;

	struct OFT *oftp = running->fd[fd];
	int dev = oftp->mip->dev;
	int ino = oftp->mip->ino;
	int offset = oftp->offset;
	int lblk  = offset / BLKSIZE;
	int start = offset % BLKSIZE;
	int byteswritten = 0;
	int size = oftp->mip->ip.i_size;

	if(nbytes<(BLKSIZE-start))
		byteswritten=nbytes;
	else
		byteswritten=BLKSIZE-start;

	oftp->mip->ip.i_size+=byteswritten;

	int blk;

	// get datablock
	//int blk = oftp->mip->ip.i_block[lbk];

	int *i_blocks = (int *)oftp->mip->ip.i_block;
	int  i, j, k, l, m;
	char buf[BLKSIZE];
	char ibuf[BLKSIZE];
	char iibuf[BLKSIZE];
	i=lblk;
	j=i;
	if(i>65803){
		m=14;
		l=((i-65803)/256)/256;
		k=((i-65803)/256)%256;
		j=(i-65803)%256;

		if(i_blocks[m]==0)
			i_blocks[m]=writeblock(oftp);
		get_block(dev,  i_blocks[m], iibuf);
		int *ininindir=(int *)iibuf;

		if(ininindir[l]==0)
			ininindir[l]=writeblock(oftp);
		get_block(dev, ininindir[l],  ibuf);
		int   *inindir=(int *) ibuf;

		if(inindir[k]==0)
			inindir[k]=writeblock(oftp);
		get_block(dev,   inindir[k],   buf);
		int     *indir=(int *)  buf;

		if(indir[j]==0)
			indir[j]=writeblock(oftp);
		blk=indir[j];

		strcpy(buf, (char *)indir);
		put_block(dev, inindir[k], buf);
		strcpy(ibuf, (char *)inindir);
		put_block(dev, ininindir[l], ibuf);
		strcpy(iibuf, (char *)ininindir);
		put_block(dev, i_blocks[m], iibuf);
	}
	else if(i>267){
		l=13;
		k=(i-267)/256;
		j=(i-267)%256;

		if(i_blocks[l]==0)
			i_blocks[l]=writeblock(oftp);
		get_block(dev,  i_blocks[l],  ibuf);
		int   *inindir=(int *) ibuf;

		if(inindir[k]==0)
			inindir[k]=writeblock(oftp);
		get_block(dev,   inindir[k],   buf);
		int     *indir=(int *)  buf;

		if(indir[j]==0)
			indir[j]=writeblock(oftp);
		blk=indir[j];

		strcpy(buf, (char *)indir);
		put_block(dev, inindir[k], buf);
		strcpy(ibuf, (char *)inindir);
		put_block(dev, i_blocks[l], ibuf);
	}
	else if(i>11){
		k=12;
		j=(i-12);
		if(i_blocks[k]==0)
			i_blocks[k]=writeblock(oftp);
		printf("out\n");
		printf("block=%d\n", i_blocks[k]);
		get_block(dev, 	i_blocks[k],   buf);
		int     *indir=(int *)  buf;

		if(indir[j]==0)
			indir[j]=writeblock(oftp);
		blk=indir[j];
		strcpy(buf, (char *)indir);
		put_block(dev, i_blocks[k], buf);
	}
	else{
		if(i_blocks[j]==0)
			i_blocks[j]=writeblock(oftp);
		blk=i_blocks[j];
	}

	char dbuf[BLKSIZE];//raw data block
	//if((oftp->mip->ip.i_blocks/2)<lblk){
	//balloc(dev);
	//oftp->mip->ip.i_blocks+=2;
	//	strcpy(dbuf, "");
	//}
	//else
	get_block(dev, blk, dbuf);

	//if(WR)
	//	iput(oftp->mip);
	//char tbuf[BLKSIZE];// temporary
	//strncpy(tbuf, dbuf, start);// an extra bit is written at the beggining
	//tbuf[strlen(tbuf)-1]=0;// This removes that extra bit
	strncat(dbuf, wbuf, byteswritten);
	//strcpy(dbuf, tbuf);
	if(debug)
		printf("writing \"%s\" to disk\n", dbuf);
	put_block(dev, blk, dbuf);

	//if(strlen(wbuf)>byteswritten){
	//	strncpy( wbuf, wbuf[byteswritten], strlen(wbuf)-byteswritten);
	//}
	//else
	//	strcpy(wbuf, "");

	oftp->offset+=byteswritten;
	if(debug){
		printf("nbytes=%d\n", nbytes);
		printf("offset=%d\n", oftp->offset);
		printf("byteswritten=%d\n", byteswritten);
	}
	oftp->mip->dirty=1;
	if(strlen(wbuf)<byteswritten)
		return byteswritten-strlen(wbuf);
	return byteswritten;
}
writehelper(int fd, char *buf, int nbytes){
	char tbuf[strlen(buf)];//temporary buf
	int i;
	for(i=0;i<strlen(buf);i++)
		tbuf[i]=0;
	strncpy(tbuf, buf, strlen(buf));
	int n=0;
	while(nbytes>0){
		n=mywrite(fd, buf, nbytes);
		if(n<0){
			printf("File not open for writing\n");
			return;
		}
		nbytes-=n;
		if(nbytes>0){
			strncpy(tbuf, &buf[n], strlen(buf)-n);
			strcpy(buf, tbuf);
			if(debug){
				printf("nbytes=%d\n", nbytes);
				printf("looping with %s\n", buf);
			}
		}
		if(debug)
			printf("END OF BUF\n");
	}
}
write_file(){
	int fd = getNum(pathname);
	if(fd<0){
		printf("Usage: write <fd> <text>\n");
	}
	char strval[BLKSIZE];
	if(WR)
		writehelper(fd, parameter, strlen(parameter));
}
cat_file(){
	int fd = myopen(pathname, 0);
	if(fd<0)
		return;
	int n;
	char buf[BLKSIZE];

	int c;
	while(n=myread(fd, buf, BLKSIZE)){
		for(c=0;c<n;c++)
			putchar(buf[c]);
		getchar();
	}
	putchar('\n');

	myclose(running, fd);
}
mycp(char *path, char *param){
	if(!strlen(param)||!strlen(path)){
		printf("usage: cp <file> <newfile>\n");
		return;
	}
	int fd = myopen(path, 0);
	int gd = myopen(param, 1);

	//cat_file();

	if(fd<0){
		printf("invalid source\n");
		return;
	}
	if(gd<0){
		printf("creating file\n");
		make(0, param);
		gd = myopen(param, 1);
	}

	char buf[BLKSIZE];
	int n;
	printf("reading\n");
	while(n=myread(fd, buf, BLKSIZE)){
		if(debug)
			printf("coppying %d \"%s\"...\n", n, buf);
		if(n==-1){
			printf("ERROR\n");
			return;
		}
		mywrite(gd, buf, n);
	}
	myclose(running, fd);
	myclose(running, gd);
	// read file in chunks
	// print it in chucnks in new file
}
cp_file(){
	mycp(pathname, parameter);
}
mymv(char *path, char *param){
	int dev0;
	int dev1;

	/*if(path[0]=='/')
		dev0=root->dev;
	else
		dev0=running->cwd->dev;
	if(param[1]=='/')
		dev1=root->dev;
	else
		dev1=running->cwd->dev;*/

	dev0 = getdev(running->cwd->dev, path);
	dev1 = getdev(running->cwd->dev, param);
	// LOOK THROUGH THE PATH
		// IF ANOTHER DEV IS ON THE PATH
			// CHANGE THE DEV
	if(dev0==dev1){
		ln(1, path, param);
		rm(2, path);
	}
	else{
		mycp(path, param);
		rm(2, path);
	}
}
mv_file(){
	mymv(pathname, parameter);
}
