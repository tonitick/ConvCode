#ifndef __CHANNEL__H
#define __CHANNEL__H

#include "Noise.h"

class Channel {
public:
	Channel(Noise * noi, int cap = 1 << 20);
	~Channel();
	bool send(const double* signal, int n);
	bool receive(double* signal, int n);
private:
	bool could_send;
	bool could_receive;
	unsigned capacity;
	unsigned carrying;
	double *signal;
	Noise *noise;
};

#endif
