#include <cstdio>
#include <vector>
#include <cstring>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include "useful.hpp"

class Swarm;
class Individual {
friend Swarm;
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
		// 適応度は今の所パラメータの平均にする
		rate = 0.0;
		for(const auto i: params) rate += i;
		rate /= params.size();
	}

	void print() const {
		fprintf(stderr, "params = ");
		for(const auto i: params) fprintf(stderr, "%lf ", i);
		fprintf(stderr, "\n");
		fprintf(stderr, "rate = %lf\n", rate);
	}

	inline double getRate() {
		return rate;
	}
};

class Swarm {
private:
	std::vector<Individual *> swarm;
	XorOshiro128p rand;
	double cumulativeSum;

	Swarm(std::vector<Individual *> swarm): swarm(swarm), cumulativeSum(0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
	}

	void shuffleSwarm() {
		for(size_t i = 0; i < swarm.size(); i++) {
			size_t j = rand(swarm.size() - 1);
			Individual *work = swarm[i];
			swarm[i] = swarm[j];
			swarm[j] = work;
		}
	}

	void samplingWithoutReplacement(size_t num, std::vector<Individual *> &res) {
		res = std::vector<Individual *>(num);
		double cumulativeSumBuf = cumulativeSum;
		for(size_t i = 0; i < num; i++) {
			// size_t pos = rand.gend(cumulativeSumBuf);
			size_t pos = 0;
			size_t j = 0;
			double rateSum = 0;
			while(rateSum <= pos && i +  j < swarm.size()) {
				rateSum += swarm[i + j++]->rate;
			}
			res[i] = swarm[i + j - 1];
			cumulativeSumBuf -= swarm[i + j - 1]->rate;
			std::swap(swarm[i], swarm[i + j - 1]);
		}
	}

public:
	Swarm(size_t size): cumulativeSum(0) {
		swarm = std::vector<Individual *>(size, nullptr);
		for(size_t i = 0; i < size; i++) swarm[i] = new Individual();

		std::random_device seed;
		rand = XorOshiro128p(seed());
	}
	
	Swarm() {
	}

	~Swarm() {
		for(size_t i = 0; i < swarm.size(); i++) delete swarm[i];
	}

	void eval() {
		cumulativeSum = 0;
		for(auto i: swarm) {
			i->eval();
			cumulativeSum += i->rate;
		}
	}

	Individual *getElite() {
		Individual *elite;
		double ratebuf = -1.0;
		for(auto i: swarm) {
			if(i->rate > ratebuf) {
				ratebuf = i->rate;
				elite = i;
			}
		}
		return elite;
	}

	Swarm *makeNextSwarm() {
		std::vector<Individual *> nextSwarm(swarm.size(), nullptr);
		eval();
		shuffleSwarm();
		fprintf(stderr, "------ Swarm --------------------------\n");
		for(const auto i: swarm) {
			i->print();
			fprintf(stderr, "addr = %p\n", i);
		}

		// エリート
		nextSwarm[0] = new Individual(getElite()->params);

		// ルーレット
		for(size_t i = 1; i < nextSwarm.size(); i++) {
			std::vector<Individual *> p;

			samplingWithoutReplacement(2, p);

			nextSwarm[i] = p[0]->makeNewIndividual(p[1]);
		}

		return new Swarm(nextSwarm);
	}

	void print() {
		for(const auto i: swarm) i->print();
	}
};

int main() {
	std::chrono::system_clock::time_point start, end;
	/*	start = std::chrono::system_clock::now();
		end = std::chrono::system_clock::now();
		double time = (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);
		printf("time: %lf\n", time);
		*/

	size_t Np = 4;
	size_t Ng = 1;
	Swarm *swarm, *nextSwarm;

	swarm = new Swarm(Np);

	for(size_t i = 0; i <= Ng; i++) {	
		fprintf(stderr, "----------------------------------------\n");
		fprintf(stderr, "%ld世代目!!\n", i);

		if(i == Ng) {
			fprintf(stderr, "------ Final --------------------------\n");
		}

		nextSwarm = swarm->makeNextSwarm();

		printf("%ld %lf\n", i, swarm->getElite()->getRate());
		delete swarm;
		swarm = nextSwarm;
	}

	/*
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
		if(buf[0] == 'w') printf("Win Mine\n");
		else if(buf[0] == 'l') printf("Lose Mine\n");
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
	*/
}

