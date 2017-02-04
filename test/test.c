#include "test1.c"
#include <unistd.h>

int func(int x)
{
	return x;
}
char *stringos(char bluh[64])
{
	char blah[64];
	strcpy(blah, bluh);
}
typedef struct stct{char *var; int *x;}fuckstruct;
struct stct globalgroup[3];
struct stct *refer;
main(int argc, char **argv, char **env){

	printf("Enter test\n");

	/*
	if(access("test1.c", F_OK))
		printf("no test1.c\n");
	else
		printf("test1.c exists!\n");

	int pid = fork();
	int status;
	if(!pid)
		execv("otherprog", argv);//, env);
	else
		pid = wait(&status);

	struct fuck fug;
	fug.shit = 'f';

	struct stct group[2];

	group[0].var = "group[a]";
	group[1].var = "group[b]";

	globalgroup[0].var = "globalgroup[c]";
	globalgroup[1].var = "globalgroup[d]";
	globalgroup[2].var = "globalgroup[e]";

	int (*funcptr)(int);
	funcptr = &func;
	int x = funcptr(1);
	printf("%d \n", x);
	printf("%s, %s\n", group[0].var, group[1].var);
	printf("%s, %s, %s\n", globalgroup[0].var, globalgroup[1].var, globalgroup[2].var);
	printf("%c\n", fug.shit);
	char *s[16];
	char *p[32];
	printf("gibbe de input\n");
	scanf("%s %s", s, p);
	printf("s: %s; p: %s\n", s, p);
	printf(stringos("blahblahblah\n"));
	fuckstruct *referr;
	referr->var = "fucku\n";
	printf(referr->var);
	*/
	int i=3;
	//scanf("%d\n", i);
	switch(i){
		case 0:
		case 1:
			printf("blah\n");
			break;
		case 2:
		case 3:
			printf("bluh\n");
			break;
		//case 2:
		//	printf("bluh\n");
		//	break;
		//case 3:
		//	printf("bluh\n");
		//	break;
		default:
			printf("screw u\n");
			break;
	}
	//refer->var = foo;
	//printf("referencing stuff %s", refer->var);
}

