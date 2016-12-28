#ifndef __NOISE_H__
#define __NOISE_H__

#include "RandNum.h"

class Noise {
public:
	virtual void generateNoise(double* noise_data, int n) = 0;
};

class WhiteGaussianNoise:public Noise {
public:
	WhiteGaussianNoise(double mu, double sigma);
	virtual void generateNoise(double* noise_data, int n);

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

private:
	CWHRandNum generator;
	double mean;
	double deviation;
};

#endif
