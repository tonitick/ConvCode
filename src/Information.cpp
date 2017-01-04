#include "Information.h"

void Information::generateInfoSeq(int* info_seq, int n) {
	generator.SetSeed(0);
	for(int i = 0; i < n; i++) {
		if(generator.Uniform() < 0.5)
			info_seq[i] = 0;
		else
			info_seq[i] = 1;
	}
}

void Information::modulateInfo(double* modulated_info, int* input_info, int n) {
	for(int i = 0; i < n; i++) {
		if(input_info[i] == 0)
			modulated_info[i] = 1.0;
		else
			modulated_info[i] = -1.0;
	}
}
