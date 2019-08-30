#include <cstdio>
#include <random>
#include "useful.hpp"

class Swarm;
class Individual {
friend Swarm;
private:
	XorOshiro128p rand;
	double rate;
	std::vector<double> params;

public:
	Individual(): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		for(size_t i = 0; i < 10; i++) params.emplace_back(rand.gend());
	}

	void eval() {
		// 勝率を今の所ランダムにする
		rate = 0.0;
		for(const auto i: params) rate += i;
		rate /= params.size();
	}

	void print() const {
		printf("params = ");
		for(const auto i: params) printf("%lf ", i);
		printf("\n");
		printf("rate = %lf\n", rate);
	}
};

class Swarm {
private:
	std::vector<Individual *> swarm;
	size_t size;
	bool freeFlag;
	XorOshiro128p rand;

	Individual *chooseFromSwarm() {
		std::vector<double> cum(size, 0.0);
		printf("cum = ");
		for(size_t i = 0; i < size; i++) cum[i] = swarm[i]->rate + ((i == 0) ? 0 : cum[i - 1]);
		for(size_t i = 0; i < size; i++) {
			cum[i] /= cum[size - 1];
			printf("%lf ", cum[i]);
		}
		printf("\n");
		double choose = rand.gend();
		printf("choose = %lf\n", choose);
		size_t chooseInd;
		for(chooseInd = 0; chooseInd < size; chooseInd++) if(choose < cum[chooseInd]) break;
		printf("chooseInd = %ld\n", chooseInd);

		return swarm[chooseInd];
	}

public:
	Swarm(size_t size): freeFlag(true), size(size) {
		swarm = std::vector<Individual *>(size, nullptr);
		for(size_t i = 0; i < size; i++) swarm[i] = new Individual();

		std::random_device seed;
		rand = XorOshiro128p(seed());
	}

	Swarm(): freeFlag(false), size(0) {
	}

	~Swarm() {
		if(freeFlag) for(auto i: swarm) delete i;
	}

	void eval() {
		for(auto i: swarm) i->eval();
	}

	void print() {
		for(const auto i: swarm) i->print();
		chooseFromSwarm();
	}
};

int main() {
	int n = 3;
	Swarm swarm(n);

	swarm.eval();
	swarm.print();

	return 0;
}

