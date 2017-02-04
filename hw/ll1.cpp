#include <string>
#include <string.h>
#include <cstdio>

class node
{
	public:
	char type;//d or f // n = null
	char name[16];//64
	node *childptr;
	node *siblingptr;
	node *parentptr;
	node(){}
	node(char nodetype, char *nodename)
	{
		type = nodetype;
		strcpy(name, nodename);
	}
	void init()
	{
		childptr = new node('n', "null");
		siblingptr = new node('n', "null");
		parentptr = new node('n', "null");
	}
};

node *newnode(char nodetype, char *nodename)
{
	node *retval = new node(nodetype, nodename);
	retval->init();
	return retval;
}
node *newnode(char nodetype, char *nodename, node *parentnode)
{
	node *retval = newnode(nodetype, nodename);
	retval->parentptr = parentnode;
	return retval;
}
/*
struct node initNode(char nodetype, char nodename[64])
{
	struct node newnode;
	newnode.type = nodetype;
	strcpy(newnode.name, nodename);
	// *newnode.childptr = node_default;
	// *newnode.siblingptr = node_default;
	// *newnode.parentptr = node_default;
	return newnode;
}*/

class Command
{
	public:
	char Name[16];
	void(*funcptr)();
	Command(){}
	Command(char *comname, void (*comfun)())
	{
		strcpy(Name,comname);
		funcptr = comfun;
	}
};
