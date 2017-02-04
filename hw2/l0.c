typedef unsigned int u32;

mymain(int argc, char *argv[], char *env[])
{
	char *s = "( I don't want to retype all of this text and I don't know how to copy+paste, so I'm going to leave it in this order, sorry if that's inconvenient )\n";
	myprints(s);
	u32 x = 1024;
	int y = -1024;
	s = "Test of myprints(\"hello world!\"):\nhello world!\n";
	myprints(s);
	s = "Test of print functions (u, d, o, x)\n";
	myprints(s);
	s = "\nprintu(100) = ";
	myprints(s);
	printu(x);
	s = "\nprintd(100) = ";
	myprints(s);
	printd(y);
	s = "\nprinto(100) = ";
	myprints(s);
	printo(x);
	s = "\nprintx(100) = ";
	myprints(s);
	printx(x);
	putchar('\n');
	
	s = "Test of myprintf(\"Char: %c, Str: %s, u32: %u, Dec: %d, Oct: %o, Hex: %x\\n\", \'a\', \"Hello World!\", 100, 100, 100, 100)\n";
	myprints(s);
	s = "Char: %c, Str: %s, u32: %u, Dec: %d, Oct: %o, Hex: %x\n";
	char a = 'c';
	char *b = "Hello World!";
	u32 c = 100;
	int d = 100;
	u32 e = 100;
	u32 f = 100;
	myprintf(s, a, b, c, d, e, f);



	int i;

	myprintf("in mymain(): argc=%d\n", argc);

	for(i = 0; i < argc; i++)
		myprintf("argv[%d] = %s\n", i, argv[i]);

	if(argc < 2)
		for(i = 0; env[i] != 0; i++)
		{
			myprintf("%s\n", env[i]);
		}

	
}
