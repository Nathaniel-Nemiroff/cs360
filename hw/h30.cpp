#include "ll0.cpp"
#include <cstdio>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int ARGC;
char **ARGV;
char **ENV;


void getArg(char *output)
{
	if(ARGC > 2)
		strcpy(output, ARGV[2]);
}
void clearfile()
{
	FILE *fp = fopen("hw3tree.nnt", "w");
	fprintf(fp, "e end");
	fclose(fp);
}
void myProgramhw3(int argc, char *argv[], char *env[])
{
	ARGC = argc;
	ARGV = argv;
	ENV  = env;
	myinitialize("hw3tree.nnt");
	getDirname = getArg;
	myreload();
	int com;
	if(argc > 1)
	{
		com = findCmd(argv[1]);
		if(!strcmp(argv[1], "clear"))
		{
			clearfile();
			return;
		}
	}
	if(com >-1)
		cmd[com].funcptr();
	else
	{
		printf("This is %d ( Parent =%d )\n", getpid(), getppid());
		int pid = fork();
		if(pid < 0)
		{
			perror("fork failed");
			exit(1);
		}
		if(pid)
			printf("This is %d ( Parent =%d )\n", getpid(), getppid());
		else
			printf("I am %d ( Parent =%d )\n", getpid(), getppid());
	}
	mysave();
}
