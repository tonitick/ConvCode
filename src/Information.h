#ifndef __INFOMATION_H__
#define __INFOMATION_H__

#include "RandNum.h"

class Information {
 private:
    CLCRandNum generator;

 public:
 	void generateInfoSeq(int* info_seq, int n);
 	void modulateInfo(double* modulated_info, int* input_info, int n);
};

#endif