#include "CConvCodec.h"
#include "stdio.h"
#include <math.h>
#include <string.h>

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

	//decoding
	for(int i = 0; i < m_len_trellis; i++) {
		getGamma(in_bit_probs, i);

		//calculate the probability of all input respectively
		double* prob = new double[m_num_input];
		memset(prob, m_num_input, sizeof(double));
		for(int j = 0; j < m_num_edge; j++) {
			prob[m_in_label[j]] = prob[m_in_label[j]] + 
			                      m_alpha[i][m_left_vertex[j]] * 
			                      m_beta[i + 1][m_right_vertex[j]] * 
			                      m_gamma[m_left_vertex[j]][m_right_vertex[j]];
		}
		
		//get decoded element
		int dec = -1;
		double max_prob = -1.0;
		for(int j = 0; j < m_num_input; j++) {
			if(prob[j] > max_prob) {
				dec = j;
			}
		}
		for(int j = 0; j < m_num_input; j++) {
			uu_hat[i * m_num_input + j] = dec % 2;
			dec = dec / 2;
		}		
	}
}

void CConvCodec::getAlpha(double *in_bit_probs) {
	//set marginal value
	m_alpha[0][0] = 1;
	for(int i = 1; i < m_num_state; i++) {
		m_alpha[0][i] = 0;
	}

	for(int forward = 1; forward < m_len_trellis + 1; forward++) {
		//calculate gamma
		getGamma(in_bit_probs, forward - 1 );

		//calculate alpha l
		for(int i = 0; i < m_num_state; i++) {
			m_alpha[forward][i] = 0;
			for(int j = 0 ;j < m_num_state; j++) {
				m_alpha[forward][i] = m_alpha[forward][i] + m_alpha[forward - 1][j] * m_gamma[j][i];
			}
		}
	}
}

void CConvCodec::getBeta(double *in_bit_probs) {
	//set marginal value
	for(int i = 0; i < m_num_state; i++) {
		m_beta[m_len_trellis][0] = 1;
	}

	for(int backward = m_len_trellis - 1; backward >= 0; backward--) {
		//calculate gamma
		getGamma(in_bit_probs, backward);

		//calculate beta l
		for(int i = 0; i < m_num_state; i++) {
			m_beta[backward][i] = 0;
			for(int j = 0 ;j < m_num_state; j++) {
				m_beta[backward][i] = m_beta[backward][i] + m_beta[backward + 1][j] * m_gamma[i][j];
			}
		}
	}
}

void CConvCodec::getGamma(double *in_bit_probs, int index) {
	for(int i = 0; i < m_len_trellis; i++) {
		for(int j = 0; j < m_len_trellis; j++) {
			for(int k = 0; k < m_num_edge; k++) {
				if(m_left_vertex[k] == i && m_right_vertex[k] == j) {
					m_gamma[i][j] = 0.5;
					int out_label = m_out_label[k];
					for(int l = 0; l < m_len_input; l++) {
						if(out_label % 2 == 0)
							m_gamma[i][j] = m_gamma[i][j] * in_bit_probs[index * m_len_input + l];
						else
							m_gamma[i][j] = m_gamma[i][j] * (1 - in_bit_probs[index * m_len_input + l]);
					}
					break;
				}
				if(k == m_num_edge)
					m_gamma[i][j] = 0.0;
			}
		}
	}	
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

	printf("trellis:\n");
	printf("m_len_trellis = %d\n", m_len_trellis);
	for(int i = 0; i < m_num_edge; i++) {
		printf("%d %d %d %d\n", m_left_vertex[i], 
			                    m_in_label[i], 
			                    m_out_label[i], 
			                    m_right_vertex[i]);
	}

}