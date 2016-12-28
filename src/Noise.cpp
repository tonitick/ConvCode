#include "Noise.h"

WhiteGaussianNoise::WhiteGaussianNoise(double mu, double sigma) {
	mean = mu;
	deviation = sigma;
}


void WhiteGaussianNoise::generateNoise(double* noise_data, int n) {
	generator.Normal(noise_data, n);
	for (int i = 0; i < n; i++) {
		noise_data[i] = deviation * noise_data[i] + mean;
	}
}
