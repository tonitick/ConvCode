#include "Runner.h"

Runner::Runner(int tot, int codl, double es, double dev, char* p) {
	total_len = tot;
	code_len = codl;
	Es = es;
	deviation = dev;
	
	path = new char[strlen(p) + 1];
	strncpy(path, p, strlen(p) + 1);
	FILE* fp;
	fp = fopen(path, "r+");
	fscanf(fp, "%d%d", &code_no, &out_no);
	out_len = code_len / code_no * out_no;

	fclose(fp);
}

Runner::~Runner() {
	if(path != NULL)
		delete path;
}

int Runner::getErrorNum(int* original, int* decoded, int n) {
	int sum = 0;
	for(int i = 0; i < n; i++) {
		if(original[i] != decoded[i]) {
			sum++;
		}
	}

	return sum;
}


void Runner::process() {
	//initialize
	Information info;
	WhiteGaussianNoise wgn(0, deviation);
	Channel AWGN(&wgn);
	CConvCodec conv;
	conv.malloc(code_len, code_no, path);
	
	int* info_seq = new int[code_len];
	int* coded_seq = new int[out_len];
	double* modulated_seq = new double[out_len];
	double* prob = new double[out_len];
	double* rec = new double[out_len];
	int* decoded_seq1 = new int[code_len];
	int* decoded_seq2 = new int[code_len];
	
	int group = total_len / code_len;
	int errorNum1 = 0;
	int errorNum2 = 0;	
	
	for(int round = 0; round < group; round++) {
		//generate Information Sequence
		info.generateInfoSeq(info_seq, code_len);

		//encode
		conv.encoder(info_seq, coded_seq);

		//modulate
		info.modulateInfo(modulated_seq, coded_seq, out_len);

		//sent and receive
		AWGN.send(modulated_seq, out_len);
		AWGN.receive(rec, out_len);

		//calculate prob
		for(int i = 0; i < out_len; i++) {
			if(rec[i] > 0)
				prob[i] = 1.0 / (1.0 + exp(-2.0 * rec[i] / (deviation * deviation)));
			else
				prob[i] = 1.0 - 1.0 / (1.0 + exp(2.0 * rec[i] / (deviation * deviation)));
		}

		//decode
		conv.softInHardOut(prob, decoded_seq1);
		conv.softInHardOut2(rec, Es, 2 * deviation * deviation, decoded_seq2);
		
		//error num
		errorNum1 = errorNum1 + getErrorNum(info_seq, decoded_seq1, code_len);
		errorNum2 = errorNum2 + getErrorNum(info_seq, decoded_seq2, code_len);

		// printSeq(decoded_seq1, code_len);
		// printSeq(decoded_seq2, code_len);
	}
	printf("error rate of first method= %.10lf\n", double(errorNum1) / double(code_len * group));
	printf("error rate of second method= %.10lf\n", double(errorNum2) / double(code_len * group));

	delete []info_seq;
	delete []coded_seq;
	delete []modulated_seq;
	delete []prob;
	delete []rec;
	delete []decoded_seq1;
	delete []decoded_seq2;
}