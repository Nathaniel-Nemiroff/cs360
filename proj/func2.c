// LEVEL 3

//#include "search.c"

#define BLKSIZE 1024
extern int Ldebug;

isMounted(char *path){
	int i;
	for(i=0;i<10;i++){
		if(mounttable[i]->dev>0){
			if(!strcmp(mounttable[i]->name, path))
				return 1;
		}
	}
	return 0;
}
getMount(struct MINODE *mip){
	int i;
	for(i=0;i<10;i++){
		if(mounttable[i]->dev>0)
			if(mounttable[i]->mounted_inode->dev==mip->dev){
				if(mounttable[i]->mounted_inode->ino==mip->ino){
					return i;//mounttable[i]->dev;
				}
			}
	}
	return -1;
}
devToMount(int dev){
	int i;
	for(i=0;i<10;i++)
		if(mounttable[i]->dev==dev)
			return i;
	return -1;
}
isBusy(char *path){
	int ret=0;
	int i;
	for(i=0;i<100;i++){
		if(minode[i]->refCount>0){
			char check[128];
			if(debug)
				printf("pwd %d %d %d...\n", i, minode[i]->ino, minode[i]->dev);
			strcpy(check, pwdrec(&minode[i]->ip, 0, minode[i]->dev));
			if(debug){
				printf("pwd successful\n");
				printf("check=%s\n", check);
				printf("path=%s\n", path);
			}
			if(strstr(check, path)!=NULL){/*
				int d = mounttable[devToMount(minode[i]->dev)]->dev;
				int k =0;
				int j;
				printf("starting\n");
				for(j=0;j<100;j++)
					if(minode[j]->refCount>0)
						if(minode[j]->dev==d){
							printf("minode[%d]\n", i);
							k+=minode[j]->refCount;
						}
				printf("returning %d for %d\n", k, d);
				return k;*/
				ret+=minode[i]->refCount;
			}
		}
	}
	return ret;
}
printMinodes(){
	int i;
	printf("Used minodes...\n");
	for(i=0;i<100;i++){
		if(minode[i]->refCount>0){
			char check[128];
			strcpy(check, pwdrec(&minode[i]->ip, 0, minode[i]->dev));
			if(!strlen(check))
				strcpy(check, "/");
			printf("dev=%d ino=%2d ", minode[i]->dev, minode[i]->ino);
			printf("refCount=%d ", minode[i]->refCount);
			printf("%s\n", check);
		}
	}
}
mount(){
	if(!strlen(pathname)){
		printf("Mounted devices:\n");
		printf("     name        dev  mounted path\n");
		int i;
		for(i=0;i<10;i++){
			if(mounttable[i]->dev>0){
				char p[128];
strcpy(p, pwdrec(&mounttable[i]->mounted_inode->ip, 0, mounttable[i]->mounted_inode->dev));
				if(!strlen(p))
					strcpy(p, "/");
				printf("%14s   ", mounttable[i]->name);
				printf(" %d ", mounttable[i]->dev);
				printf("   %s", p);
				printf("\n");
			}
		}
		return 0;
	}
	if(isMounted(pathname)){
		if(debug)
			printf("Disk already mounted\n");
		return -1;
	}

	if(!strlen(parameter)){
		printf("No specified mount point!\n");
		return -1;
	}
	if(isBusy(parameter)){
		printf("Directory is busy\n");
		return -1;
	}

	int i;
	for(i=0;i<10;i++){
		if(mounttable[i]->dev==0){
			break;
		}
	}
	if(debug==10){
		printf("Reached Disk limit!\n");
		return -1;
	}
	if(debug)
		printf("Opening %s...\n", pathname);

	int ino = getino(running->cwd->dev, parameter);
	int dev = getdev(running->cwd->dev, parameter);
	int fd = open(pathname, O_RDWR);
	if(debug)
		printf("result: fd=%d\n", fd);
	if(fd < 0){
		printf("Open failed\n");
		return -1;
	}
	if(!ino){
		printf("Mounting point not found\n");
		return -1;
	}
	if(debug)
		printf("Open succeeded!\n");


	char buf[BLKSIZE];
	get_block(fd, 2, buf);
	GD *gp = (GD *)buf;
	char sbuf[BLKSIZE];
	get_block(fd, 1, sbuf);
	SUPER *sp = (SUPER *)sbuf;

	if(debug)
	printf("disk %d \"magic number\" =%x\n", fd, sp->s_magic);
	if(sp->s_magic != 0xEF53){
		printf("NOT an EXT2 FS\n");
		return -1;
	}

	if(debug)
		printf("Preparing mount entry...\n");

	mounttable[i]=(struct MOUNT *)malloc(sizeof(struct MOUNT));

	mounttable[i]->nblocks=sp->s_blocks_count;
	mounttable[i]->ninodes=sp->s_inodes_count;
	mounttable[i]->bmap=gp->bg_block_bitmap;
	mounttable[i]->imap=gp->bg_inode_bitmap;
	mounttable[i]->iblk=gp->bg_inode_table;

	mounttable[i]->mounted_inode = (struct MINODE *)malloc(sizeof(struct MINODE));
	mounttable[i]->mounted_inode = iget(dev, ino);
	mounttable[i]->dev=fd;

	strcpy(mounttable[i]->name, pathname);

	mounttable[i]->mounted_inode->mountptr = (struct MOUNT *)malloc(sizeof(struct MOUNT));
	mounttable[i]->mounted_inode->mountptr = mounttable[i];

	return 0;
}
umount(){
	//int ino = getino(running->cwd->dev, pathname);
	//int dev = getdev(running->cwd->dev, pathname);
	//printf("ino=%d dev=%d\n", ino, dev);
	//struct MINODE *mip = iget(dev, ino);
	//int m = getMount(mip);
	//iput(mip);
	if(!strlen(pathname)){
		printf("Usage: umount <diskname> or umount <dir>\n");
		return -1;
	}
	char path[128];
	if(pathname[0]!='/'){
		strcpy(path, pwdrec(&running->cwd->ip, 0, running->cwd->dev));
		if(!strlen(path))
			strcpy(path, "/");
		strcat(path, pathname);
	}
	else
		strcpy(path, pathname);
	int i;
	char p[128];
	for(i=0;i<10;i++){
		if(mounttable[i]->dev>0){
			if(debug)
				printf("m[%d]->dev=%d\n", i, mounttable[i]->dev);
			strcpy(p, pwdrec(&mounttable[i]->mounted_inode->ip, 0, mounttable[i]->mounted_inode->dev));
			if(!strcmp(p, path))
				break;
			if(!strcmp(mounttable[i]->name, pathname))
				break;
			}
	}
	if(i==10){
		printf("device not mounted\n");
		return -1;
	}
	if(isBusy(p)>1){
		printf("device is busy!\n");
		return -1;
	}

	close(mounttable[i]->dev);
	iput(mounttable[i]->mounted_inode);
	mounttable[i]->dev=0;

	return 0;
}
cs(){
printf("Not implemented yet\n");
}
do_fork(){
//printf("Not implemented yet\n");
	int i;
	for(i=0;i<100;i++)
		if(proc[i]->uid<0){
			proc[i]->uid=running->uid;
			proc[i]->pid=running->pid;
			proc[i]->cwd = icopy(running->cwd);
			int j;
			for(j=0;i<10;j++){
				if(running->fd[j]->refCount>0){
					proc[i]->fd[j]=running->fd[i];
					proc[i]->fd[j]->mip->refCount++;
				}
			}
			break;
		}
	running = proc[i];
}
do_ps(){
	int i;
	for(i=0;i<100;i++)
		if(proc[i]->uid==running->uid)
			printf("pid=%d cwd=(dev=%d ino=%d)\n", i, proc[i]->cwd->dev, proc[i]->cwd->ino);
}
