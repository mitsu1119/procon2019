#include <cstdio>
#include <random>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
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
	XorOshiro128p rand;

public:
	Family(Individual *p1, Individual *p2) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		family.emplace_back(p1);
		family.emplace_back(p2);
		family.emplace_back(p1->makeNewIndividual(p2));
		family.emplace_back(p1->makeNewIndividual(p2));
		for(auto i: family) i->eval();
	}

	Individual *chooseAndPop() {
		std::vector<double> cum(family.size(), 0.0);
		for(size_t i = 0; i < family.size(); i++) cum[i] = family[i]->rate + ((i == 0) ? 0 : cum[i - 1]);
		for(size_t i = 0; i < family.size(); i++) {
			if(cum[family.size() - 1] == 0) cum[i] = 1.0 / family.size();
			else cum[i] /= cum[family.size() - 1];
		}
		double choose = rand.gend();
		size_t chooseInd;
		for(chooseInd = 0; chooseInd < family.size(); chooseInd++) if(choose < cum[chooseInd]) break;
		
		Individual *retn = family[chooseInd];		// vec.erase(n) だと遅い
		family[chooseInd] = family.back();
		family.pop_back();
		return retn;
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

	Swarm(std::vector<Individual *> swarm): swarm(swarm), freeFlag(true) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
	}

	void shuffleSwarm() {
		for(size_t i = 0; i < swarm.size(); i++) {
			size_t j = rand(swarm.size() - 1);
			/* LP64 とかそのあたりの環境依存
			Individual *work = swarm[i];
			swarm[i] = swarm[j];
			swarm[j] = work;
			*/
			swarm[i] = (Individual *)((unsigned long)swarm[i] ^ (unsigned long)swarm[j]);
			swarm[j] = (Individual *)((unsigned long)swarm[j] ^ (unsigned long)swarm[i]);
			swarm[i] = (Individual *)((unsigned long)swarm[i] ^ (unsigned long)swarm[j]);
		}
	}

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

	Swarm *makeNextSwarm() {
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
		return new Swarm(nextSwarm);
	}

	void print() {
		for(const auto i: swarm) i->print();
	}
};

int main() {
	/*
	int n = 4;
	size_t depth = 1;
	Swarm *swarm, *nextSwarm;
	swarm = new Swarm(4);

	for(size_t i = 0; i < depth; i++) {
		nextSwarm = swarm->makeNextSwarm();
		delete swarm;
		swarm = nextSwarm;
	}
	printf("---- Final Swarm -------------------------\n");
	swarm->print();
	*/

	int fd[2];
	char buf[100];
	memset(buf, 0, sizeof(buf));	// buf clear.

	if(pipe(fd) < 0) {
		perror("PIPE ERROR");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	switch(pid) {
	case -1:
		// fork error.
		err(EXIT_FAILURE, "Could not fork.");
		break;
	case 0: {
		// child process.
		dup2(fd[0], 0);
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		char *const args[] = {"./run", NULL};
		execv(args[0], args);
		break;
			}
	default:
		// parents
		int childStatus;
		printf("This is parent process. child process number is %d\n", pid);

		pid_t wait_pid = wait(&childStatus);
		
		read(fd[0], buf, sizeof(buf));
		if(buf[0] == 'W') printf("Win Mine\n");
		else if(buf[0] == 'L') printf("Lose Mine\n");
		else printf("Draw\n");
		close(fd[0]);
		close(fd[1]);
		if(WIFEXITED(childStatus)) {
			printf("Finished uccessfully. child = %d, status = %d\n", wait_pid, WEXITSTATUS(childStatus));
		} else if(WIFSIGNALED(childStatus)) {
			printf("Child process %d ended by signale %d\n", wait_pid, WTERMSIG(childStatus));
		} else {
			err(EXIT_FAILURE, "wait err");
			exit(EXIT_FAILURE);
		}
	}
	
	return 0;
}

