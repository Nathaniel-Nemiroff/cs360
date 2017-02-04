#include <stdio.h>
#include <netdb.h>

//#include <arpa/inet.h>

main(int argc, char *argv[])
{
	struct hostent *lh = gethostbyname("localhost");

	if(lh)
		puts((char *)inet_ntoa(lh->h_addr));
	else
		herror("gethostbyname");
	return 0;
}
