#include <stdio.h>

double test(double x);
double fib(double x);
double fibi(double x);

double printd(double x)
{
	putchar((char)x);
	return 0;
}

int main()
{
	test(65);
	test(97);

	printf("\n");

	printf("fib\tfibi\n");
	for (unsigned i = 1; i < 10; i++)
		printf("%g\t%g\n", fib(i), fibi(i));

	return 0;
}