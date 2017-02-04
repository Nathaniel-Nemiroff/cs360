main(int argc, char *argv[])
{
	printf("%s; starting new a.out...\n", argv[1]);
	execl("a.out", "a.out", argv[1]+1, 0);
}
