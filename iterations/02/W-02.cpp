int main() {
	int a = 1;
	{
		int a = 2;
		((void) a);
	}
	((void) a);

	int b = 1;
	{
		int b = 2;
		((void) b);
	}
	((void) b);

	return 0;
}
