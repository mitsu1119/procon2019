#include <cstdio>
#include <random>
#include <vector>
#include <algorithm>
#include "useful.hpp"

class Swarm;
class Family;
class Individual {
friend Swarm;
friend Family;
private:
	XorOshiro128p rand;
	double rate;
	size_t dim;
	std::vector<double> params;

	Individual(std::vector<double> parameters): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		params = parameters;
		dim = params.size();
	}

	Individual *makeNewIndividual(const Individual *p) {
		std::vector<double> genParam;
		double dist, max, min;
		for(size_t i = 0; i < dim; i++) {
			dist = std::abs(p->params[i] - params[i]);
			max = std::max(p->params[i], params[i]) + 0.3 * dist;
			min = std::min(p->params[i], params[i]) - 0.3 * dist;
			dist = std::abs(max - min);
			genParam.emplace_back((dist + min) * rand.gend());
		}
		Individual *indiv = new Individual(genParam);
		return indiv;
	}

public:
	Individual(): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		for(size_t i = 0; i < 10; i++) params.emplace_back(rand.gend());
		dim = params.size();
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

class Family {
private:
	std::vector<Individual *> family;

public:
	Family(Individual *p1, Individual *p2) {
		family.emplace_back(p1);
		family.emplace_back(p2);
		family.emplace_back(p1->makeNewIndividual(p2));
		family.emplace_back(p1->makeNewIndividual(p2));
	}

	Individual *chooseAndPop() {
		return family[0];
	}

	void print() const {
		printf("family -----------------------------------------\n");
		printf("          [parent]\n");
		family[0]->print();
		family[1]->print();
		printf("          [child]\n");
		family[2]->print();
		family[3]->print();
		printf("------------------------------------------------\n");
	}
};

class Swarm {
private:
	std::vector<Individual *> swarm;
	bool freeFlag;
	XorOshiro128p rand;

	void shuffleSwarm() {
		for(size_t i = 0; i < swarm.size(); i++) {
			size_t j = rand(swarm.size() - 1);
			Individual *work = swarm[i];
			swarm[i] = swarm[j];
			swarm[j] = work;
		}
	}

/*
	std::vector<Individual *> chooseFromSwarm(size_t N) {
		if(N != 0) std::vector<Individual *> retn = chooseFromSwarm(N - 1);
		std::vector<double> cum(size, 0.0);
		for(size_t i = 0; i < size; i++) cum[i] = swarm[i]->rate + ((i == 0) ? 0 : cum[i - 1]);
		for(size_t i = 0; i < size; i++) {
			cum[i] /= cum[size - 1];
		}

		size_t chooseInd;
		double choose = rand.gend();
		std::vector<bool> choosed(size, false);
			for(chooseInd = 0; chooseInd < size; chooseInd++) if(choose < cum[chooseInd]) break;
			if(choosed[chooseInd]) goto reChoose;
			else choosed[chooseInd] = true;
			retn.emplace_back(swarm[chooseInd]);
			printf("chooseInd = %ld\n", chooseInd);	

		return retn;	
*/

public:
	Swarm(size_t size): freeFlag(true) {
		swarm = std::vector<Individual *>(size, nullptr);
		for(size_t i = 0; i < size; i++) swarm[i] = new Individual();

		std::random_device seed;
		rand = XorOshiro128p(seed());
	}

	Swarm(): freeFlag(false) {
	}

	~Swarm() {
		if(freeFlag) for(auto i: swarm) delete i;
	}

	void eval() {
		for(auto i: swarm) i->eval();
	}

	std::vector<Individual *> makeNextSwarm() {
		std::vector<Individual *> nextSwarm(2 * (swarm.size() / 2), nullptr);
		shuffleSwarm();
		printf("------ Swarm --------------------------\n");
		for(const auto i: swarm) {
			i->print();
			printf("addr = %p\n", i);
		}
		size_t cnt = 0;
		while(swarm.size() / 2 > 0) {
			Family family(swarm[swarm.size() - 1], swarm[swarm.size() - 2]);
			swarm.pop_back();
			swarm.pop_back();
			nextSwarm[cnt++] = new Individual(*family.chooseAndPop());
			nextSwarm[cnt++] = new Individual(*family.chooseAndPop());
		}
		printf("----- next Swarm -----------------------\n");
		for(const auto i: nextSwarm) {
			i->print();
			printf("addr = %p\n", i);
		}
		return nextSwarm;
	}

	void print() {
		for(const auto i: swarm) i->print();
	}
};

int main() {
	int n = 4;
	Swarm swarm(n);

	swarm.makeNextSwarm();

	return 0;
}

