class CConvCodec {
 public:
    CConvCodec();
    virtual ~CConvCodec();
    
    //convolution code info
    int m_codedim;
    int m_codelen;
    int m_codechk;
    double m_coderate;
    
    //input and out sequences info
    int m_len_input; //number of the inputs
    int m_num_input; //total number of the possible inputs
    int m_len_output; //number of the outputs
    int m_num_output; //total number of the possible outputs
    
    //Trellis info
    int m_num_state; //number of the states
    int m_num_edge; //number of the edges
    
    //Trellis
    int *m_left_vertex;
    int *m_in_label;
    int *m_out_label;
    int *m_right_vertex;
    int m_len_trellis; //length of the trellis
    
    //arguments for BCJR algorithm
    double **m_alpha;
    double **m_beta;
    double **m_gamma;
    
    void Malloc(int len_uu, int code_no, char *file_name);
    void Free();
    void Encoder(int *uu, int *cc);
    void SoftInHardOut(double *in_bit_probs, int *uu_hat);
};