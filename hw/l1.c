#include <string.h>

typedef struct node
{
	char type;//d or f // n = null
	char name[64];//64
	struct node *childptr;
	struct node *siblingptr;
	struct node *parentptr;

	
}Node;

Node node_default = {'n', "null", NULL, NULL, NULL};

struct node initNode(char nodetype, char nodename[64])
{
	struct node newnode;
	newnode.type = nodetype;
	strcpy(newnode.name, nodename);
	//*newnode.childptr = node_default;
	//*newnode.siblingptr = node_default;
	//*newnode.parentptr = node_default;
	return newnode;
}

typedef struct Command
{
	char *Name;
	int(*funcptr)();
}Comm;
