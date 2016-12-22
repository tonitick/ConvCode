#include "Information.h"
#include "Noise.h"
#include "Channel.h"

int main() {
	Information ii;
	WhiteGaussianNoise wn(0, 1);
	Channel AGWN(&wn);
}