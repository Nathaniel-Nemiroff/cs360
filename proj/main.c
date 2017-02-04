// copy the text on KC's website: hhtp://eecs.wsu.edu/~cs360/mountroot.html

//#include "mainfunc.c"
#include "func.c"
//#include "func1.c"
//#include "func2.c"

//struct PROC *running;

int debug, WR, Ldebug;
extern int fd;
extern struct PROC *running;
extern struct PROC *proc[100];
extern char pathname[128], parameter[60];


doCmd(char cname[64])
{
	if(!strcmp(cname, "menu")){
		menu();
		return 1;}
	if(!strcmp(cname, "mkdir")){
		make_dir();
		return 1;}
	if(!strcmp(cname, "cd")){
		change_dir();
		return 1;}
	if(!strcmp(cname, "pwd")){
		pwd();
		return 1;}
	if(!strcmp(cname, "ls")){
		list_dir();
		return 1;}
	if(!strcmp(cname, "rmdir")){
		rmdir();
		return 1;}
	if(!strcmp(cname, "creat")){
		creat_file();
		return 1;}
	if(!strcmp(cname, "link")){
		link();
		return 1;}
	if(!strcmp(cname, "unlink")){
		unlink();
		return 1;}
	if(!strcmp(cname, "symlink")){
		symlink();
		return 1;}
	if(!strcmp(cname, "rm")){
		rm_file();
		return 1;}
	if(!strcmp(cname, "chmod")){
		chmod_file();
		return 1;}
	if(!strcmp(cname, "chown")){
		chown_file();
		return 1;}
	if(!strcmp(cname, "stat")){
		stat_file();
		return 1;}
	if(!strcmp(cname, "touch")){
		touch_file();
		return 1;}
	if(!strcmp(cname, "open")){
		open_file();
		return 1;}
	if(!strcmp(cname, "close")){
		close_file();
		return 1;}
	if(!strcmp(cname, "pfd")){
		pfd();
		return 1;}
	if(!strcmp(cname, "lseek")){
		lseek_file();
		return 1;}
	if(!strcmp(cname, "access")){
		access_file();
		return 1;}
	if(!strcmp(cname, "read")){
		read_file();
		return 1;}
	if(!strcmp(cname, "write")){
		write_file();
		return 1;}
	if(!strcmp(cname, "cat")){
		cat_file();
		return 1;}
	if(!strcmp(cname, "cp")){
		cp_file();
		return 1;}
	if(!strcmp(cname, "mv")){
		mv_file();
		return 1;}
	if(!strcmp(cname, "mount")){
		mount();
		return 1;}
	if(!strcmp(cname, "umount")){
		umount();
		return 1;}
	if(!strcmp(cname, "cs")){
		cs();
		return 1;}
	if(!strcmp(cname, "fork")){
		do_fork();
		return 1;}
	if(!strcmp(cname, "ps")){
		do_ps();
		return 1;}
	if(!strcmp(cname, "sync")){
		sync();
		return 1;}
	if(!strcmp(cname, "quit")){
		quit();
		return 1;}
	if(!strcmp(cname, "lazydebug")){
		Ldebug=1;
		return 1;
	}
	if(!strcmp(cname, "min")){
		if(Ldebug){
			printMinodes();
			return 1;
		}
	}
	printf("Invalid command!\n");
	return 0;
}

int main(int argc, char *argv[])
{
   int i, cmd;
   char line[128], cname[64];
	debug = 0;
	WR = 0;
	if(argc > 2){
		if(!strcmp(argv[2], "-d"))
			debug = 1;
		if(!strcmp(argv[2], "-w"))
			WR = 1;
	}
	if(argc >3){
		if(!strcmp(argv[3], "-d"))
			debug = 1;
		if(!strcmp(argv[3], "-w"))
			WR = 1;
	}
	WR = 1;
	if(debug)
		printf("debugging");
	if(debug && WR)
		printf(" and ");
	if(WR && debug)
		printf("writing");
	if(debug || WR)
		printf("\n");
	Ldebug=0;

   char disk[64] = "mydisk";
   if(argc > 1){
	   strncpy(disk, argv[1], 64);
   }
   if(init(disk) == -1)
	   exit(1);

  running = (struct PROC *)malloc(sizeof(struct PROC));
  running = proc[0];
  running->pid = proc[0]->pid;
  running->cwd = iget(fd, 2);

  if(debug){
  printf("running->pid = %d\n", running->pid);
  printf("running->cwd->ip->i_size=%d\n", running->cwd->ip.i_size);
  printf("running->cwd->ino=%d\n", running->cwd->ino);
  }

	if(debug)
		printf("Successfully initialized!\n");

	printf("Welcome to Nathan's EXT2 FS reader!\n");

	char path[128];

   while(1){
	strcpy(pathname, "");
	strcpy(parameter, "");
	printf("P%d:", running->pid);
	if(debug)
		printf("dev=%d ino=%d:", running->cwd->dev, running->cwd->ino);
	//printf("input command : ");
	strcpy(path, pwdrec(&running->cwd->ip, 0, running->cwd->dev));
	if(!strlen(path))
		strcpy(path, "/");
	printf("%s : ", path);
	fgets(line, 128, stdin);
	line[strlen(line)-1] = 0; // kill the \r char at the end
	if(line[0]==0) continue;

	char lone[128];
	strcpy(lone, line);
	char *lane;
	int index=0;
	lane=strtok(lone, " ");
	index+=strlen(lane);
	lane=strtok(NULL, " ");
	if(lane!=NULL){
		index+=strlen(lane)+1;
		lane=strtok(NULL," ");
		if(lane!=NULL)
			index=(strlen(line)-index)-1;
		else
			index=0;
	}

	sscanf(line, "%s %s %60c", cname, pathname, parameter);
	parameter[index]=0;

	cmd = doCmd(cname); // map cname to an index

	/////////////////////////////////////////////////////
	// MUST CHANGE THIS TO A TABLE OF FUNCTION POINTERS//
	/////////////////////////////////////////////////////
	/*
	switch(cmd){
	   / ************** LEVEL 1 ************* /
	   case  0 : menu();		break;
	   case  1 : make_dir();	break;
	   case  2 : change_dir();	break;
	   case  3 : pwd();		break;
	   case  4 : list_dir();	break;
	   case  5 : rmdir();		break;
	   case  6 : creat_file();	break;
	   case  7 : link();		break;
	   case  8 : unlink();		break;
	   case  9 : symlink();		break;
	   case 10 : rm_file();		break;
	   case 11 : chmod_file();	break;
	   case 12 : chown_file();	break;
	   case 13 : stat_file();	break;
	   case 14 : touch_file();	break;

	   / ************** LEVEL 2 ************* /
	   case 20 : open_file();	break;
	   case 21 : close_file();	break;
	   case 22 : pfd();		break;
	   case 23 : lseek_file();	break;
	   case 24 : access_file();	break;
	   case 25 : read_file();	break;
	   case 26 : write_file();	break;
	   case 27 : cat_file();	break;
	   case 28 : cp_file();		break;
	   case 29 : mv_file();		break;

	   / ************** LEVEL 3 ************* /
	   case 30 : mount();		break;
	   case 31 : umount();		break;
	   case 32 : cs();		break;
	   case 33 : do_fork();		break;
	   case 34 : do_ps();		break;

	   case 40 : sync();		break;
	   case 41 : quit();		break;
	   default : printf("invalid command\n");
		break;

	}
	*/
   }
}
