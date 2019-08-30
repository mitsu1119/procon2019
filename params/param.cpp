#include <cstdio>
#include <random>
#include "useful.hpp"

class Individual {
private:
	XorOshiro128p rand;
	double rate;
public:
	Individual(): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
	}

	void eval() {
		rate = rand.gend();
	}

	void print() {
		printf("rate = %lf\n", rate);
	}
};

int main() {
	int n = 3;
	Individual *swarm = new Individual[n];

	for(size_t i = 0; i < n; i++) swarm[i].print();
	for(size_t i = 0; i < n; i++) swarm[i].eval();
	for(size_t i = 0; i < n; i++) swarm[i].print();

	delete [] swarm;
	return 0;
}

