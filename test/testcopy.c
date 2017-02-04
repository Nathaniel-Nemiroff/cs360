#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main(){
	char buf[8];

	char wbuf[64];

	strcpy(buf, "blah\n");

	strcpy(wbuf, "these words are not in buf to begin with\n");

	strcat(buf, wbuf);

	printf("%s\n", buf);
}
