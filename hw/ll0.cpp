#include <cstdio>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "ll1.cpp"
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ostream *out;
istream *in;
fstream *ifile;
fstream *ofile;

node *root, *cwd;
node root_t, cwd_t;
char line[128];
char command[16], pathname [64];
char dirname[64], mybasename[64];
Command cmd[14];
char argus[16][16];
int cmdnum = 14;
char quit;
char *filename;

node *findNodeRec(char *nodename, char nodetype, node *index)
{
	if(strcmp(index->siblingptr->name, "null"))
	{
		if(!strcmp(index->siblingptr->name, nodename))
		{
			if(index->siblingptr->type == nodetype)
				return index->siblingptr;
		}
		return findNodeRec(nodename, nodetype, index->siblingptr);
	}
	return newnode('n', "null");//nullnode;
}
node *findNode(char *nodename, char nodetype)
{
	if(!strcmp(nodename, "root"))
		return root;
	if(strcmp(cwd->childptr->name, "null"))
	{
		if(!strcmp(cwd->childptr->name, nodename))
		{
			if(cwd->childptr->type == nodetype)
				return cwd->childptr;
		}
		return findNodeRec(nodename, nodetype, cwd->childptr);
	}
	return newnode('n', "null");//nullnode;
}
int setCWD(char *path)
{
	node cwdtmp = *cwd;
	char *dir = strtok(path, "/");
	while(dir != NULL)
	{
		cwdtmp = *cwd;
		cwd = findNode(dir, 'd');
		if(!strcmp(cwd->name, "null"))
		{
			*cwd = cwdtmp;
			return -1;
		}
		//printf("%s\n", dir);
		dir = strtok(NULL, "/");
	}
	return 0;
}
node *findNullRec(node *index)
{
	if(!strcmp(index->siblingptr->name, "null"))
		return index;
	return findNullRec(index->siblingptr);
}
node *findNull()
{
	if(!strcmp(cwd->childptr->name, "null"))
		return cwd;
	return findNullRec(cwd->childptr);
}

char *mypwdRec(node *index)
{
	char *name = index->name;
	char path[64] = {""};
	if(strcmp(index->parentptr->name, "null"))
	{
		while(strcmp(index->parentptr->siblingptr->name, "null"))
		{
			if(index->parentptr->siblingptr == index)
				index = index->parentptr;
			else
				break;
		}
		strcpy(path, mypwdRec(index->parentptr));
	}
	//if(strcmp(name, "root"))
	//{
		strcat(path, "/");
		strcat(path, name);
	//}
	return path;
}

void savePath(node *index, FILE *fp)
{
	char *name = index->name;
	if(strcmp(index->parentptr->name, "null"))
	{
		while(strcmp(index->parentptr->siblingptr->name, "null"))
		{
			if(index->parentptr->siblingptr == index)
				index = index->parentptr;
			else
				break;
		}
		savePath(index->parentptr, fp);
	}
	fprintf(fp, "/%s", name);
}
void saveTree(node *index, FILE *fp)
{
	fprintf(fp, "%c ", index->type);
	//savePath(index, fp);
	fprintf(fp, mypwdRec(index));
	fprintf(fp, "\n");
	node *children = index->childptr;
	while(strcmp(children->name, "null"))
	{
		saveTree(children, fp);
		children = children->siblingptr;
	}
}

void addNode(char *path, char type)
{
	char *dir = strtok(path, "/");
	while(dir != NULL)
	{
		if(strcmp(findNode(dir, type)->name, "null"))
			cwd = findNode(dir, type);
		else if(strcmp(dir, "root"))
		{
			if(!strcmp(cwd->childptr->name, "null"))
				cwd->childptr = newnode(type, dir, cwd);
			else
			{
				node *parent = findNullRec(cwd->childptr);
				parent->siblingptr = newnode(type, dir, parent);
			}
			break;
		}
		dir = strtok(NULL, "/");
	}
	
}

void loadTree(FILE *fp)
{
	char type[1];
	char Dirname[16];
	fscanf(fp, "%s %s", type, dirname);
	while(type[0] != 'e')
	{
		cwd = root;
		addNode(Dirname, type[0]);
		fscanf(fp, "%s %s", type, Dirname);	
	}
}

void getInput(char *output)
{
	strcpy(output, argus[1]);
}
void (*getDirname)(char *output);

void myPrint(char *input)
{
	*out << input;
}
void myPrint(int input)
{
	*out << input;
}

void mymkdir()
{
	getDirname(dirname);

	if(!strcmp(dirname, "null"))
		myPrint("Error: directory name cannot be \"null\"");
	else if(!strcmp(cwd->childptr->name, "null"))
		cwd->childptr = newnode('d', dirname, cwd);
	else
	{
		node *parent = findNullRec(cwd->childptr);
		parent->siblingptr = newnode('d', dirname, parent);
	}
}//serach for null
void myrmdir()
{
	getDirname(dirname);
	
	node *rmnode = findNode(dirname, 'd');
	if(!strcmp(rmnode->name, "null"))
		myPrint("Error: Directory not found\n");
	else
	{
		if(rmnode->parentptr == cwd)
			cwd->childptr = rmnode->siblingptr;
		else
			rmnode->parentptr->siblingptr = rmnode->siblingptr;
	}
}//serach for object
void myls()
{
	if(strcmp(cwd->childptr->name, "null"))// != NULL
	{
		myPrint(cwd->childptr->name);
		myPrint(" ");
		node *index = cwd->childptr;
		while(strcmp(index->siblingptr->name, "null"))
		{
			myPrint(index->siblingptr->name);
			myPrint(" ");
			index = index->siblingptr;
		}
	}
	myPrint("\n");
}//find all
void mycd()
{
	getDirname(dirname);
	
	node *index = findNode(dirname, 'd');
	if(!strcmp(dirname, ".."))
	{
		if(cwd != root)
		{
			while(cwd->parentptr->siblingptr == cwd)
				cwd = cwd->parentptr;
			cwd = cwd->parentptr;//actually gets it one level up
		}
		else
			myPrint("Error: no parent directory\n");
	}
	else if(strcmp(index->name, "null"))
		cwd = index;
	else
		myPrint("Error: Directory not found\n");
}//search for object
void mypwd()
{
	char path[64];
	strcpy(path, mypwdRec(cwd));
	myPrint(path);
	myPrint("\n");
}//display entire path
void mycreat()
{
	getDirname(dirname);

	if(!strcmp(dirname, "null"))
		myPrint("Error: directory name cannot be \"null\"");
	else if(!strcmp(cwd->childptr->name, "null"))
		cwd->childptr = newnode('f', dirname, cwd);
	else
	{
		node *parent = findNullRec(cwd->childptr);
		parent->siblingptr = newnode('f', dirname, parent);
	}
}//search for null
void myrm()
{
	getDirname(dirname);
	
	node *rmnode = findNode(dirname, 'f');
	if(!strcmp(rmnode->name, "null"))
		myPrint("Error: file not found\n");
	else
	{
		if(rmnode->parentptr == cwd)
			cwd->childptr = rmnode->siblingptr;
		else
			rmnode->parentptr->siblingptr = rmnode->siblingptr;
	}
}//search for object
void myquit()
{
	quit = 't';
	ifile->close();
	ofile->close();
}//nothing
void myhelp()
{
	myPrint("List of commands:\n");
	for(int i = 0; i < cmdnum; i++)
	{
		myPrint(cmd[i].Name);
		myPrint("\n");
	}
}//list commands
void myquestion(){myhelp();}//???
void mymenu(){myhelp();}//list commands?
/*
char *myReload()
{
	FILE *fp = fopen(filename, "r");
	loadTree(fp);
	char path[64];
	fscanf(fp, "%s", path);
	fclose(fp);
	cwd = root;
	return path;
}*/ //reinitialize node tree
void myreload()
{
	FILE *fp = fopen(filename, "r");
	loadTree(fp);
	char path[64];
	fscanf(fp, "%s", path);
	fclose(fp);
	//cwd = findNode(path, 'd');//root;
	setCWD(path);
	//cwd = root;
	//return path;
}//reinitialize node tree
void mysave()
{
	FILE *fp = fopen(filename, "w");
	saveTree(root, fp);
	fprintf(fp, "e end ");//end
	fprintf(fp, mypwdRec(cwd));
	fclose(fp);
}//save node tree
void mynull(){}//pootis

void myinitialize(char *FileName)
{
	quit = 'f';
	out  = &cout;
	in   = &cin;
	ifile = new fstream();
	ofile = new fstream();

	cmd[0]  = *(new Command("mkdir",  &mymkdir));
	cmd[1]  = *(new Command("rmdir",  &myrmdir));
	cmd[2]  = *(new Command("ls",     &myls));
	cmd[3]  = *(new Command("cd",     &mycd));
	cmd[4]  = *(new Command("pwd",    &mypwd));
	cmd[5]  = *(new Command("creat",  &mycreat));
	cmd[6]  = *(new Command("rm",     &myrm));
	cmd[7]  = *(new Command("quit",   &myquit));
	cmd[8]  = *(new Command("help",   &myhelp));
	cmd[9]  = *(new Command("?",      &myquestion));
	cmd[10] = *(new Command("menu",   &mymenu));
	cmd[11] = *(new Command("reload", &myreload));
	cmd[12] = *(new Command("save",   &mysave));
	cmd[13] = *(new Command("null",   &mynull));
	
	root = newnode('d', "root");
	cwd = root;
	filename = FileName;
	getDirname = getInput;	
}
int findCmd(char *command)
{
	int i = 0;
	while(i < cmdnum)
	{
		if(strcmp(command, cmd[i].Name) == 0)
			return i;
		i++;
	}
	return -1;
}
void myProgramhw2(int argc, char *argv[], char *env[])
{
	myinitialize("mytree.nnt");	
	//printf("%s, %s\n", cwd->name, cwd->childptr->name);
	// mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, quit
	//printf("%s, %s\n", cwd_t.name, cwd_t.childptr->name);
	int com;//mand
	int pid;
	int printcom = 0;
	char oend[64] = "";
	
	
	
	while(quit == 'f')
	{
		char pwdrec[64];
		strcpy(pwdrec, mypwdRec(cwd));

		myPrint(pwdrec);
		myPrint(": ");
		//myPrint(oend);

		char tokens[64];
		char *comm;

		in->getline(tokens, sizeof tokens);
		if(printcom)
		{
			strcpy(oend, tokens);
			strcat(oend, "\n");
		}

		myPrint(oend);

		if(strcmp(tokens, ""))
		{
			comm = strtok(tokens, " ");
			com = findCmd(comm);
		}
		else
		{
			comm = NULL;
			com = 13;
		}

		int i = 0;
		while(comm != NULL)
		{
			//printf("%s\n", comm);
			if(!strcmp(comm, "<"))
			{
				comm = strtok(NULL, " ");
				if(comm == NULL)
				{
					myPrint("Error: no file\n");
					return;
				}
				ifile->open(comm, fstream::in);
				in = ifile;
				printcom = 1;

				comm = strtok(NULL, " ");
				if(comm != NULL)
				{
					if(!strcmp(comm, "|"))
					{
						comm = strtok(NULL, " ");
						continue;
					}
				}

				break;
			}
			if(!strcmp(comm, ">"))
			{
				comm = strtok(NULL, " ");
				if(comm == NULL)
				{
					myPrint("Error: no file\n");
					return;
				}
				char fl[16];
				strcpy(fl, comm);
				ofile->open(fl, fstream::trunc|fstream::out);
				out = ofile;
				printcom = 1;
				break;
			}
			
			strncpy(argus[i++], comm, sizeof comm -1);
			comm = strtok(NULL, " ");
		}
		strcpy(argus[i], "null");

		if(com > -1)
			cmd[com].funcptr();
		else
		{
			pid = fork();
			int status;
			if(pid < 0)
			{
				perror("fork failed");
				exit(1);
			}
			if(pid)
			{
				pid=wait(&status);
				myPrint("exit status: ");
				myPrint(status);
				myPrint("\n");
			}
			else
			{
				char path[64];
				strcpy(path, "./");
				strcat(path, argus[0]);
				execv(path, argv);
				myPrint("Error, no file ");
				myPrint(argus[0]);
				myPrint("\n");
				exit(100);
			}
		}
	}
	
}
