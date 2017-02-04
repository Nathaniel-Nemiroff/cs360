myprintf(char *fmt, ...)
{
	int *ip = &fmt + 1;
	char *cp = fmt;
	while(*cp != '\0')
	{
		if(*cp == '%')
		{
			cp = cp + 1;
			switch(*cp)
			{
				case 'c':
					putchar(*ip);
					break;
				case 's':
					myprints(*ip);
					break;
				case 'u':
					printu(*ip);
					break;
				case 'd':
					printd(*ip);
					break;
				case 'o':
					printo(*ip);
					break;
				case 'x':
					printx(*ip);
					break;
				default:
					break;
			}
			ip = ip + 1;
		}
		else
			putchar(*cp);
		if(*cp == '\n')
			putchar('\r');
		cp = cp + 1;
	}
}
