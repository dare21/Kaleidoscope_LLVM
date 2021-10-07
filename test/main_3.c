#include <stdio.h>

double putchard(double x) {
	putchar((char)x);
	return 0.0;
}

double printstar(double);
double printstar10(double);

int main() {
	unsigned i;
	for (i = 0; i < 20; i++) {
		printstar(i);
		printf("\n");
	}

	printf("----------------------");

	for (i = 0; i < 20; i++) {
		printstar10(i);
		printf("\n");
	}

	return 0;
}