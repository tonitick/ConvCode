#include "Information.h"
#include "Noise.h"
#include "Channel.h"
#include "CConvCodec.h"
#include "RandNum.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

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


#define uu_len 500
int main() {
	srand((unsigned)time(NULL));
	// CWHRandNum generator;
	// double test[10];
	// generator.Normal(test, 10);
	// printSeq2(test, 10);
	Information ii;
	WhiteGaussianNoise wn(0, 1);
	Channel AGWN(&wn);
	CConvCodec conv;
	conv.malloc(uu_len, 1, "../test_sample/sample1.txt");
	//conv.printInfo();
	//generate information sequence
	int* info_seq = new int[uu_len];
	ii.generateInfoSeq(info_seq, uu_len);
	//printSeq(info_seq, uu_len);
	
	//encode
	int* coded_seq = new int[2 * uu_len];
	conv.encoder(info_seq, coded_seq);
	//printSeq(coded_seq, 2 * uu_len);
	
	//modulate
	double* modulated_seq = new double[2 * uu_len];
	ii.modulateInfo(modulated_seq, coded_seq, 2 * uu_len);
	//printSeq2(modulated_seq, 2 * uu_len);

	//sent and receive
	AGWN.send(modulated_seq, 2 * uu_len);
	double* rec = new double[2 * uu_len];
	AGWN.receive(rec, 2 * uu_len);
	//printSeq2(rec, 2 * uu_len);
	
	//calculate prob
	double* prob = new double[2 * uu_len];
	for(int i = 0; i < 2 * uu_len; i++) {
		prob[i] = 1.0 / (1.0 + exp(-2.0 * rec[i] / 1));
	}
	//printf("prob:\n");
	//printSeq2(prob, 2 * uu_len);

	//decode
	int* decoded = new int[uu_len];
	conv.softInHardOut(prob, decoded);
	//printSeq(decoded, uu_len);

	//error rate
	printf("%lf\n", errorRate(info_seq, decoded, uu_len));

	return 0;
}