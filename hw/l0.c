#include <stdio.h>
#include <string.h>
#include "l1.c"

struct node *root, *cwd;
char line[128];
char command[16], pathname [64];
char dirname[64], basename[64];
struct Command cmd[14];
char quit;

main()
{
	myProgram();
}

struct node *findNodeRec(char *nodename, struct node *index)
{
	if(index->siblingptr != NULL)
	{
		if(index->siblingptr->name == nodename)
			return index->siblingptr;
		return findNodeRec(nodename, index->siblingptr);
	}
	//struct node nullnode = NULL;
	//nullnode.name = "null";
	return NULL;//nullnode;
}
struct node *findNode(char *nodename)
{
	if(cwd->childptr != NULL)
	{
		if(cwd->childptr->name == nodename)
			return cwd->childptr;
		return findNodeRec(nodename, cwd->childptr);
	}
	//struct node nullnode;
	//nullnode.name = "null";
	return NULL;//nullnode;
}

struct node *findNullRec(struct node *index)
{
	if(index->siblingptr == NULL)
		return index->siblingptr;
	return findNullRec(index->siblingptr);
}
struct node *findNull()
{
	if(cwd->childptr == NULL)
		return cwd->childptr;
	return findNullRec(cwd->childptr);
}

mypwdRec(struct node *index)
{
	if(index->parentptr != NULL)
		mypwdRec(index->parentptr);
	printf("\\%s", index->name);
}

mymkdir()
{
	printf("mkdir\n");
	struct node *newdir = findNull();
	*newdir = initNode('d', pathname);
	newdir->parentptr = cwd;
}//serach for null
myrmdir(){printf("rmdir\n");}//serach for object
myls()
{
	printf("ls\n");
	printf("%s\n", cwd->name);
	printf("%c\n", cwd->type);
	printf("%s\n", cwd->childptr->name);
	if(cwd->childptr->type != 'n')// != NULL
	{
		printf("after if statement\n");
		struct node *index = cwd->childptr;
		printf("\n%s", index->name);	
		while(index->siblingptr != NULL)
		{
			printf(" %s", index->siblingptr->name);
			index = index->siblingptr;
		}
	}
	printf("\n");
}//find all
mycd()
{
	printf("cd\n");
	struct node *index = findNode(pathname);
	if(strcmp("..", pathname))
		cwd = cwd->parentptr;
	else if(index != NULL)
		cwd = index;
	else
		printf("error");
}//search for object
mypwd()
{
	printf("pwd\n");
	mypwdRec(cwd);
}//display entire path
mycreat(){printf("creat\n");}//search for null
myrm(){printf("rm\n");}//search for object
myquit(){printf("quit\n"); quit = 't';}//nothing
myhelp(){printf("help\n");}//list commands
myquestion(){printf("?\n");}//???
mymenu(){printf("menu\n");}//list commands?
myreload(){printf("reload\n");}//reinitialize node tree
mysave(){printf("save\n");}//save node tree
mynull(){printf("0\n");}//pootis







myinitialize()
{
	quit = 'f';
	cmd[0].Name = "mkdir";
	cmd[0].funcptr = &mymkdir;
	cmd[1].Name = "rmdir";
	cmd[1].funcptr = &myrmdir;
	cmd[2].Name = "ls";
	cmd[2].funcptr = &myls;
	cmd[3].Name = "cd";
	cmd[3].funcptr = &mycd;
	cmd[4].Name = "pwd";
	cmd[4].funcptr = &mypwd;
	cmd[5].Name = "creat";
	cmd[5].funcptr = &mycreat;
	cmd[6].Name = "rm";
	cmd[6].funcptr = &myrm;
	cmd[7].Name = "quit";
	cmd[7].funcptr = &myquit;
	cmd[8].Name = "help";
	cmd[8].funcptr = &myhelp;
	cmd[9].Name = "?";
	cmd[9].funcptr = &myquestion;
	cmd[10].Name = "menu";
	cmd[10].funcptr = &mymenu;
	cmd[11].Name = "reload";
	cmd[11].funcptr = &myreload;
	cmd[12].Name = "save";
	cmd[12].funcptr = &mysave;
	cmd[13].Name = "null";
	cmd[13].funcptr = &mynull;

	printf("fuck\n");
	struct node root_temp = initNode('d', "\\");
	struct node childnode = initNode('n', "null");
	root_temp.childptr = &childnode;
	root = &root_temp;
	cwd = root;
	printf("%s, ", cwd->name);
	printf("%s\n", cwd->childptr->name);
}
int findCmd(char *command)
{
	int i = 0;
	while(cmd[i].Name != "null")
	{
		if(strcmp(command, cmd[i].Name) == 0)
			return i;
		i++;
	}
	return -1;
}
myProgram()
{
	myinitialize();
	//printf("%s, %s\n", cwd->name, cwd->childptr->name);
	// mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, quit
	char *co;//mmand
	//printf("%s, %s\n", cwd->name, cwd->childptr->name);
	int com;//mand
	printf("start program\n");
	while(quit == 'f')
	{
		printf("%s, %s\n", cwd->name, cwd->childptr->name);
		//printf("%c", quit);
		printf("input a command : ");
		scanf("%s", command);
		printf("%s\n", command);
		com = findCmd(command);
		if(com > 0)
			cmd[com].funcptr();
	}
	
}


