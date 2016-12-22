#include "Channel.h" 
#include <string.h>
Channel::Channel(Noise * noi, int cap) {
	noise = noi;
	capacity = cap;
	signal = new double[capacity];
	could_send = true;
	could_receive = false;
	carrying = 0;
}

Channel::~Channel() {
	delete[] signal;
}

bool Channel::send(const double* info_seq, int n) {
	if (!could_send) return false;
	if (n > capacity) return false;
	
	carrying = n;
	
	double* noise_data = new double[n];
	noise -> generateNoise(noise_data, n);
	for(int i = 0; i < n; i++) 
		signal[i] = info_seq[i] + noise_data[i];
	
	delete noise_data;
	
	could_receive = true;
	could_send = false;
	return true;
}

bool Channel::receive(double* rec_seq, int n) {
	if (!could_receive) return false;
	if (n > carrying) return false;
	
	for(int i = 0; i < n; i++)
		rec_seq[i] = signal[i];
	
	could_receive = false;
	could_send = true;
	return true;
}
