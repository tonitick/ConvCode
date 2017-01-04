#ifndef __RUNNER__
#define __RUNNER__

#include "Information.h"
#include "Noise.h"
#include "Channel.h"
#include "CConvCodec.h"
#include "RandNum.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

class Runner {
 public:
 	Runner(int tot, int codl, double es, double dev, char* p);
 	~Runner();
 	void process();
 private:
 	int total_len;
 	int code_len;
 	int code_no;
 	int out_len;
 	int out_no;
	double Es;
	double deviation;
	char* path;

	int getErrorNum(int* original, int* decoded, int n);

	void printSeq(int* seq, int n) {
	for(int i = 0; i < n; i++) {
		printf("%d ", seq[i]);
	}
	printf("\n");
}

void printSeq2(double* seq, int n) {
	for(int i = 0; i < n; i++) {
		printf("%lf ", seq[i]);
	}
	printf("\n");
}

double errorRate(int* original, int* decoded, int n) {
	int sum = 0;
	for(int i = 0; i < n; i++) {
		if(original[i] != decoded[i]) {
			sum++;
		}
	}

	return (double)sum / (double) n; 
}
};

#endif