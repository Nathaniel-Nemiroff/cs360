// The echo client client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX 256

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr; 

int server_sock, r;
int SERVER_IP, SERVER_PORT; 


// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(server_sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n"); 
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n", 
          hp->h_name, "NULL"/*inet_ntoa(SERVER_IP)*/, SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

int ls_dir(char *dirname)
{
	DIR *John = NULL;
	struct dirent *jonny = NULL;
	char cwd[128];
	getcwd(cwd, sizeof(cwd));
	John = opendir(cwd);
	if(dirname == NULL)
		while(NULL != (jonny = (readdir(John))))
			printf("%s\n", jonny->d_name);
	else
		while(NULL != (jonny = (readdir(John))))
			if(!strcmp(jonny->d_name, dirname))
				printf("%s\n", jonny->d_name);
	return 0;
}

int mycat(char *filename)
{
	if(filename == NULL)
		return -1;
	int c;
	FILE *fp;
	fp=fopen(filename, "r");
	while((c=getc(fp))!=EOF)
		printf("%c", c);
	fclose(fp);
	return 0;
}

main(int argc, char *argv[ ])
{
  int n;
  char line[MAX], ans[MAX];

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);
  // sock <---> server
  printf("********  processing loop  *********\n");
  while (1){
    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
       exit(0);

    char tokens[MAX];
    strcpy(tokens, line);
    char *val = strtok(tokens, " ");

    char comm[MAX];
    strcpy(comm, val);
    val = strtok(NULL, " ");


     if(!strcmp(comm, "lcat")){
	if(mycat(val)==-1)
	   printf("ERROR: NO FILE\n");
     }
else if(!strcmp(comm, "lpwd"))
	{
	   char cwd[128];
	   getcwd(cwd, sizeof(cwd));
	   printf("%s\n", cwd);
	}
else if(!strcmp(comm, "lls"))
	ls_dir(val);
else if(!strcmp(comm, "lcd"))
	chdir(val);
else if(!strcmp(comm, "lmkdir"))
	mkdir(val, S_IRWXU);
else if(!strcmp(comm, "lrmdir"))
	rmdir(val);
else if(!strcmp(comm, "lrm"))
	unlink(val);
     if(comm[0] == "l")
	continue;
     if(!strcmp(comm, "ls"))
     {
	int n = write(server_sock, line, MAX);
	
	char lsscript[MAX];
	n = read(server_sock, lsscript, MAX);
	while(strcmp(lsscript, "EOL")){
		printf("%s\n", lsscript);
		n = read(server_sock, lsscript, MAX);
	}
     }
else if(!strcmp(comm, "get"))
     {
	if(val == NULL){
	   printf("ERROR: NO FILE\n");
	   continue;
	}
	n = write(server_sock, line, MAX);
	n = read(server_sock, line, MAX);
	
	char check[5];
	strncpy(check, line, 5);
	if(strcmp(check, "SIZE=")){
	   printf("ERROR: NO FILE\n");
	   continue;
	}
	int SIZE = atoi(&line[5]);
	int count = 0;

	char *fltk = strtok(val, "/");
	char *flnm[32];
	while(fltk!=NULL){// Extract the location to write into
	   strcpy(flnm, fltk);
	   fltk = strtok(NULL, "/");
	}

	FILE *fp=fopen(flnm, "w");
	while(count < SIZE){// Reading in file and writing it to disk
	   n = read(server_sock, line, MAX);
	   count += n;
	   fprintf(fp, line);
	}
	fclose(fp);
     }
else if(!strcmp(comm, "put"))
     {
	if(val == NULL){
	   printf("ERROR: NO FILE\n");
	   continue;
	}
	struct stat st;
	if(stat(val, &st)){
	   printf("ERROR: NO FILE\n");
	   continue;
	}

	FILE *fp = fopen(val, "r");
	char retsize[32];
	sprintf(retsize, "SIZE=%d", st.st_size);
	strcat(line, " ");
	strcat(line, retsize);

	n = write(server_sock, line, MAX);

	int count = 0;
	while(count < st.st_size){
		char output[MAX];
		fgets(output, MAX, fp);
		n = write(server_sock, output, MAX);
		count += n;
	}
	fclose(fp);
	continue;
     }


    // Send ENTIRE line to server
    n = write(server_sock, line, MAX);
    //printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

    // Read a line from sock and show it
    //n = read(server_sock, ans, MAX);
    //printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    //printf("%s\n", ans);
  }
}


