#include "CConvCodec.h"
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <float.h>

CConvCodec::CConvCodec() {
	m_codedim = 0;
    m_codelen = 0;
    m_codechk = 0;
    m_coderate = 0.0;

    m_len_input = 0;
    m_num_input = 0;
    m_len_output = 0;
    m_num_output = 0;

    m_num_state = 0;
    m_num_edge = 0;
	
	m_left_vertex = NULL;
    m_in_label = NULL;
    m_out_label = NULL;
    m_right_vertex = NULL;

	m_len_trellis = 0;

	m_alpha = NULL;
	m_beta = NULL;
	m_gamma = NULL;
}

CConvCodec::~CConvCodec() {
	free();
}

void CConvCodec::malloc(int len_uu, int code_no, const char *file_name) {
	free();
	
	m_codedim = len_uu;
	m_len_input = code_no;
	FILE* fp;
	fp = fopen(file_name, "r+");
	fscanf(fp, "%d%d", &m_num_state, &m_len_output);
	m_num_input = pow(2,m_len_input);
	m_num_output = pow(2,m_len_output);
	m_num_edge = m_num_state * m_num_input;

	m_left_vertex = new int[m_num_edge];
	m_in_label = new int[m_num_edge];
	m_out_label = new int[m_num_edge];
	m_right_vertex = new int[m_num_edge];

	for(int i = 0; i < m_num_edge; i++) {
		fscanf(fp, "%d%d%d%d", &m_left_vertex[i], 
			                   &m_in_label[i], 
			                   &m_out_label[i], 
			                   &m_right_vertex[i]);		
	}

	m_len_trellis = m_codedim / m_len_input;
	m_codelen = m_len_trellis * m_len_output;
	m_codechk = m_codelen - m_codedim;
	m_coderate = (double)m_codedim / (double)m_codelen;
	
	m_alpha = new double*[m_len_trellis + 1];
	m_beta = new double*[m_len_trellis + 1];
	for(int i = 0; i < m_len_trellis + 1; i++) {
		m_alpha[i] = new double[m_num_state];
		m_beta[i] = new double[m_num_state];
	}
	
	m_gamma = new double*[m_num_state];
	for(int i = 0; i < m_num_state; i++) {
		m_gamma[i] = new double[m_num_state];
	}
}

void CConvCodec::free() {
	if(m_left_vertex != NULL) {
		delete []m_left_vertex;
		m_left_vertex = NULL; 
	}
	if(m_in_label != NULL) {
		delete []m_in_label;
		m_in_label = NULL;
	}
	if(m_out_label != NULL) {
		delete []m_out_label;
		m_out_label = NULL;
	}
	if(m_right_vertex != NULL) {
		delete []m_out_label;
	}
	if(m_alpha != NULL) {
		for(int i = 0; i < m_len_trellis + 1; i++) {
			if(m_alpha[i] != NULL) {
				delete []m_alpha[i];
				m_alpha[i] = NULL;
			}
		}
		delete []m_alpha;
	}
	if(m_beta != NULL) {
		for(int i = 0; i < m_len_trellis + 1; i++) {
			if(m_beta[i] != NULL) {
				delete []m_beta[i];
				m_beta[i] = NULL;
			}
		}
		delete []m_beta;
	}
	if(m_gamma != NULL) {
		for(int i = 0; i < m_num_state; i++) {
			if(m_gamma[i] != NULL) {
				delete []m_gamma[i];
				m_gamma[i] = NULL;
			}
		}
		delete []m_gamma;
	}
}
    
void CConvCodec::encoder(int *uu, int *cc) {
	int current_state = 0; //initial state: all zeros
	
	for(int i = 0; i < m_len_trellis; i++) {
		//calculate in label
		int in_label = 0;
		int base = 1;
		for(int j = i * m_len_input; j < (i + 1) * m_len_input; j++) {
			in_label = in_label + uu[j] * base;
			base = base * 2;
		}
		
		//find out label and next state
		int out_label;
		for(int j = 0; j < m_num_edge; j++) {
			if(m_left_vertex[j] == current_state && m_in_label[j] == in_label) {
				current_state = m_right_vertex[j];
				out_label = m_out_label[j];
				break;
			}
		}

		//calculate corresponding output
		for(int j = i * m_len_output; j < (i + 1) * m_len_output; j++) {
			cc[j] = out_label % 2;
			out_label = out_label / 2;
		}
	}
}
    
void CConvCodec::softInHardOut(double *in_bit_probs, int *uu_hat) {
	//preliminary work
	getAlpha(in_bit_probs);
	getBeta(in_bit_probs);

	// printf("alpha:\n");
	// print2D(m_alpha, m_len_trellis + 1, m_num_state);
	// printf("beta:\n");
	// print2D(m_beta, m_len_trellis + 1, m_num_state);
	
	//decoding
	for(int i = 0; i < m_len_trellis; i++) {
		getGamma(in_bit_probs, i);

		//calculate the probability of all input respectively
		double* prob = new double[m_num_input];
		for(int set_zero = 0; set_zero < m_num_input; set_zero++) {
			prob[set_zero] = -DBL_MAX; //log(0)
		}
		
		for(int j = 0; j < m_num_edge; j++) {
			if(m_alpha[i][m_left_vertex[j]] == -DBL_MAX ||
			   m_beta[i + 1][m_right_vertex[j]] == -DBL_MAX ||
			   m_gamma[m_left_vertex[j]][m_right_vertex[j]] == -DBL_MAX) {
				continue;
			}
			else if(prob[m_in_label[j]] == -DBL_MAX) {
				prob[m_in_label[j]] = m_alpha[i][m_left_vertex[j]] + 
			                          m_beta[i + 1][m_right_vertex[j]] + 
			                          m_gamma[m_left_vertex[j]][m_right_vertex[j]];
			}
			else {
				prob[m_in_label[j]] = maxStar(prob[m_in_label[j]], m_alpha[i][m_left_vertex[j]] +
					                                               m_beta[i + 1][m_right_vertex[j]] + 
					                                               m_gamma[m_left_vertex[j]][m_right_vertex[j]]);
			}
		}
		
		// printf("prob %d: ", i);
		// printSeq2(prob, m_num_input);
		
		//get decoded element
		int dec = -1;
		double max_prob = -DBL_MAX;
		for(int j = 0; j < m_num_input; j++) {
			if(prob[j] > max_prob) {
				dec = j;
				max_prob = prob[j];
			}
		}
		for(int j = 0; j < m_len_input; j++) {
			uu_hat[i * m_len_input + j] = dec % 2;
			dec = dec / 2;
		}

		delete []prob;
	}
}

void CConvCodec::getAlpha(double *in_bit_probs) {
	//set marginal value
	m_alpha[0][0] = 0; //log(1)
	for(int i = 1; i < m_num_state; i++) {
		m_alpha[0][i] = -DBL_MAX; //log(0)
	}

	for(int forward = 1; forward < m_len_trellis + 1; forward++) {
		//calculate gamma
		getGamma(in_bit_probs, forward - 1);
		// printf("gamma %d:\n", forward - 1);
		// print2D(m_gamma, m_num_state, m_num_state);
		//calculate alpha l
		for(int i = 0; i < m_num_state; i++) {
			m_alpha[forward][i] = -DBL_MAX; //log(0)
			for(int j = 0; j < m_num_state; j++) {
				if(m_alpha[forward - 1][j] == -DBL_MAX || m_gamma[j][i] == -DBL_MAX) {
					continue;
				}
				else if(m_alpha[forward][i] == -DBL_MAX) {
					m_alpha[forward][i] = m_alpha[forward - 1][j] + m_gamma[j][i];
				}
				else {
					m_alpha[forward][i] = maxStar(m_alpha[forward][i], 
						                          m_alpha[forward - 1][j] + m_gamma[j][i]);
				}
			}
		}
	}
}

void CConvCodec::getBeta(double *in_bit_probs) {
	//set marginal value
	for(int i = 0; i < m_num_state; i++) {
		m_beta[m_len_trellis][i] = 0; //log(1)
	}

	for(int backward = m_len_trellis - 1; backward >= 0; backward--) {
		//calculate gamma
		getGamma(in_bit_probs, backward);

		//calculate beta l
		for(int i = 0; i < m_num_state; i++) {
			m_beta[backward][i] = -DBL_MAX; //log(0)
			for(int j = 0; j < m_num_state; j++) {
				if(m_beta[backward + 1][j] == -DBL_MAX || m_gamma[i][j] == -DBL_MAX) {
					continue;
				}
				else if(m_beta[backward][i] == -DBL_MAX) {
					m_beta[backward][i] = m_beta[backward + 1][j] + m_gamma[i][j];
				}
				else {
					m_beta[backward][i] = maxStar(m_beta[backward][i], 
						                          m_beta[backward + 1][j] + m_gamma[i][j]);
				}
			}
		}
	}
}

void CConvCodec::getGamma(double *in_bit_probs, int index) {
	for(int i = 0; i < m_num_state; i++) {
		for(int j = 0; j < m_num_state; j++) {
			int k;
			for(k = 0; k < m_num_edge; k++) {
				if(m_left_vertex[k] == i && m_right_vertex[k] == j) {
					m_gamma[i][j] = log(0.5);
					int out_label = m_out_label[k];
					for(int l = 0; l < m_len_output; l++) {
						if(out_label % 2 == 0)
							m_gamma[i][j] = m_gamma[i][j] + log(in_bit_probs[index * m_len_output + l]);
						else
							m_gamma[i][j] = m_gamma[i][j] + log(1 - in_bit_probs[index * m_len_output + l]);

						out_label = out_label / 2;
					}
					break;
				}
			}
			if(k == m_num_edge)
				m_gamma[i][j] = -DBL_MAX; //log(0)
		}
	}	
}

double CConvCodec::maxStar(double a, double b) {
	double max;
	if(a > b)
		max = a;
	else
		max = b;

	return max + log(1 + exp(-abs(a - b)));
}

void CConvCodec::printInfo() {
	printf("m_codedim = %d\n", m_codedim);
	printf("m_codelen = %d\n", m_codelen);
	printf("m_codechk = %d\n", m_codechk);
	printf("m_coderate = %lf\n", m_coderate);

	printf("m_len_input = %d\n", m_len_input);
	printf("m_num_input = %d\n", m_num_input);
	printf("m_len_output = %d\n", m_len_output);
	printf("m_num_output = %d\n", m_num_output);

	printf("m_num_state = %d\n", m_num_state);
	printf("m_num_edge = %d\n", m_num_edge);

	printf("trellis info:\n");
	printf("m_len_trellis = %d\n", m_len_trellis);
	for(int i = 0; i < m_num_edge; i++) {
		printf("%d %d %d %d\n", m_left_vertex[i], 
			                    m_in_label[i], 
			                    m_out_label[i], 
			                    m_right_vertex[i]);
	}

}