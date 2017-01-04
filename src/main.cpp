#include "Runner.h"
#include <stdio.h>

int main() {
	printf("please intput filepath(eg. ../test_sample/sample1.txt): \n");
	char* path;
	path = new char[100];
	scanf("%s", path);

	printf("please intput total information length(eg. 100000): \n");
	int total_len;
	scanf("%d", &total_len);

	printf("please intput group length(eg. 1000): \n");
	int group_len;
	scanf("%d", &group_len);

	printf("please intput Es(eg. 1.0): \n");
	double Es;
	scanf("%lf", &Es);

	printf("please intput deviation of Gaussian noise(eg. 0.5): \n");
	double dev;
	scanf("%lf", &dev);
	
	Runner myRunner(total_len, group_len, Es, dev, path);
	myRunner.process();

	delete []path;

	return 0;
}