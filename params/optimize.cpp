#include <cstdio>
#include <vector>
#include <cstring>
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
		printf("params = ");
		for(const auto i: params) printf("%lf ", i);
		printf("\n");
		printf("rate = %lf\n", rate);
	}
};

class Swarm {
private:
	std::vector<Individual *> swarm;
	XorOshiro128p rand;

	Swarm(std::vector<Individual *> swarm): swarm(swarm) {
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

public:
	Swarm(size_t size) {
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
		for(auto i: swarm) i->eval();
	}

	Swarm *makeNextSwarm() {
		std::vector<Individual *> nextSwarm(swarm.size(), nullptr);
		eval();
		shuffleSwarm();
		printf("------ Swarm --------------------------\n");
		for(const auto i: swarm) {
			i->print();
			printf("addr = %p\n", i);
		}
		int cnt = 0;
		double ratebuf = -1.0;
		Individual *elite;
		for(auto i: swarm) {
			if(i->rate > ratebuf) {
				ratebuf = i->rate;
				elite = i;
			}
		}
		for(size_t i = 0; i < nextSwarm.size(); i++) nextSwarm[i] = new Individual(elite->params);
		return new Swarm(nextSwarm);
	}

	void print() {
		for(const auto i: swarm) i->print();
	}
};

int main() {
	std::random_device seed;
	XorOshiro128p rand(seed());

	std::vector<double> vec(1000000);
	for(size_t i = 0; i < vec.size(); i++) vec[i] = rand.gend(100.50);

	DPlot<double> plot(vec);
	plot.histogram();

	/*
	size_t Np = 4;
	size_t Ng = 1;
	Swarm *swarm, *nextSwarm;

	swarm = new Swarm(Np);

	for(size_t i = 0; i < Ng; i++) {
		nextSwarm = swarm->makeNextSwarm();
		delete swarm;
		swarm = nextSwarm;
	}
	
	printf("---- Final Swarm -------------------------\n");
	swarm->eval();
	swarm->print();
	*/

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

