#include <stdio.h>

double foo() {
	return 1;
}

double bar() {
	return 2;
}

double fib(double);
double f(double);

int main() {
	unsigned i;
	for (i = 1; i < 10; i++)
		printf("%g\n", fib(i));

	printf("\n");

	printf("%g\n", f(-2)); //0
	printf("%g\n", f(2));  //1
	printf("%g\n", f(12)); //2

	return 0;
}