typedef unsigned int u32;

int BASE = 10;
char *table = "0123456789ABCDEF";

int rpu(u32 x)
{
	char c;
	if(x)
	{
		c = table[x % BASE];
		rpu(x / BASE);
		putchar(c);
	}
}

int printu(u32 x)
{
	if(x == 0)
		putchar('0');
	else
		rpu(x);
	putchar(' ');
}

int printd(int x)
{
	BASE = 10;
	if(x < 0)
	{
		putchar('-');
		x = x * -1;
	}
	u32 y = x;
	printu(y);
}

int printo(u32 x)
{
	BASE = 8;
	putchar('0');
	printu(x);
}

int printx(u32 x)
{
	BASE = 16;
	putchar('0');
	putchar('x');
	printu(x);
}

