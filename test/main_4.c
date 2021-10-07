#include <stdio.h>

double f(double x);
double g(double x, double y);
double h(double x, double y);
double fja(double x);

double printd(double x) {
	putchar((char) x);
	return 0;
}

int main()
{
	double i;
	for (i = 0; i < 21; i += 5)
		printf("%g\n", f(i));
	
	printf("\n");

	printf("%g\n", g(1, 2));
	printf("%g\n", g(0, 2));
	printf("%g\n", g(3, 0));
	printf("%g\n", g(0, 0));

	printf("\n");

	printf("%g\n", h(1, 2));
	printf("%g\n", h(1, 1));

	printf("\n");

	fja(100);

	return 0;
}