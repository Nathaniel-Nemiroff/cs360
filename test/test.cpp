#include <cstdio>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;

class fuck
{
	public:
	char *x;
	fuck(char *y){x = y;};
};
int main()
{
	printf("fuck you");
	fuck *me = new fuck("fuck off");
	//me = new fuck("fuck off");
	//*me = fuck("fuck off");
	printf("%s\n", me->x);
	FILE *fp = fopen("smh.fam", "w");
	fprintf(fp, "blahblahblah");
	fclose(fp);
	string fug;
	const char *fog;
	printf("input pls: ");
	getline(cin, fug);
	fog = fug.c_str();
	printf("%s\n", fog);
	//cout << fug + "\n";
	char blah[16];
	char bleh[16];
	printf("before\n");
	strcpy(blah, "blah");
	printf("blah\n");
	strcpy(bleh, blah);
	printf("bleh\n");
	return 0;
}
