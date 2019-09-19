/*
 * 交叉法の条件
 * 一つの群生の数を Np = swarm.size()
 * 一つの個体の遺伝子長 Dim = individual.params.size()
 *
 * BLX-alpha + elite: Np >= 2
 *
 * SPX + elite: Np = n * (Dim + 1) + 1, nは任意の自然数
 * 正の数が保証されないので良くないかもしれない
 *
 */
#include <cstdio>
#include <vector>
#include <cstring>
#include <cmath>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include "useful.hpp"

size_t Dim = 10;
size_t Np = 14;
size_t Ng = 150;

// 交叉法
#define CHIASMA_BLXa
// #define CHIASMA_SPX

class Swarm;
class Individual {
friend Swarm;
private:
	XorOshiro128p rand;
	double rate;
	std::vector<double> params;
	
	Individual(std::vector<double> parameters): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		params = parameters;
	}

	Individual *makeNewIndividual_BLXa(const Individual *p) {
		// TODO: 最初にメモリを確保してemplace_backをなくす
		std::vector<double> genParam;
		double dist, max, min;
		for(size_t i = 0; i < Dim; i++) {
			dist = std::abs(p->params[i] - params[i]);
			max = std::max(p->params[i], params[i]) + 0.3 * dist;
			min = std::min(p->params[i], params[i]) - 0.3 * dist;
			dist = std::abs(max - min);
			genParam.emplace_back(min + rand.gend(dist));
		}
		Individual *indiv = new Individual(genParam);
		return indiv;
	}

	void makeNewIndividual_SPX(std::vector<Individual *> &p, std::vector<Individual *> &res, size_t base) {
		if(p.size() % Dim != 0) {
			fprintf(stderr, "交叉に使用する個体の数が不正です。(SPX)\n");
			exit(EXIT_FAILURE);
		}

		// Center of gravity
		std::vector<double> g(Dim);
		for(size_t i = 0; i < Dim; i++) {
			for(size_t j = 0; j < Dim; j++) {
				g[j] += p.at(i)->params[j];
			}
		}
		for(size_t j = 0; j < Dim; j++) {
			g[j] += params[j];
			g[j] /= Dim + 1;
		}

		// (Si)	
		double eps = std::sqrt(Dim + 2);
		std::vector<std::vector<double>> S(Dim + 1, std::vector<double>(Dim));
		for(size_t j = 0; j < Dim; j++) S[0][j] = g[j] + eps * (params[j] - g[j]);
		for(size_t i = 1; i < Dim; i++) {
			for(size_t j = 0; j < Dim; j++) {
				S[i][j] = g[j] + eps * (p.at(i - 1)->params[j] - g[j]);
			}
		}

		// (ri)
		std::vector<double> r(Dim);
		for(size_t i = 0; i < Dim; i++) r[i] = std::pow(rand.gend(), 1.0 / (double)(i + 1));

		// (Ci)
		std::vector<std::vector<double>> C(Dim + 1, std::vector<double>(Dim));
		C[0] = S[0];
		res.at(base) = new Individual(C[0]);
		for(size_t i = 1; i < Dim + 1; i++) {
			for(size_t j = 0; j < Dim; j++) {
				C[i][j] = S[i][j] + r[i - 1] * (S[i - 1][j] - S[i][j] + C[i - 1][j]);
			}
			res.at(base + i) = new Individual(C[i]);
		}
	}

public:
	Individual(): rate(0.0) {
		std::random_device seed;
		rand = XorOshiro128p(seed());
		for(size_t i = 0; i < Dim; i++) params.emplace_back(rand.gend());
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
			size_t pos = rand.gend(cumulativeSumBuf);
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

		// ルーレット
		for(size_t i = 0; i < nextSwarm.size() - 1; i++) {
			std::vector<Individual *> p;

#ifdef CHIASMA_BLXa
			// BLX-alpha 法	
			samplingWithoutReplacement(2, p);
			nextSwarm[i] = p[0]->makeNewIndividual_BLXa(p[1]);	
#endif

#ifdef CHIASMA_SPX
			// SPX 法
			samplingWithoutReplacement(Dim + 1, p);
			Individual *pp = p[Dim];
			p.pop_back();
			pp->makeNewIndividual_SPX(p, nextSwarm, i);
			i += Dim;
#endif
		}
		
		// エリート
		nextSwarm[nextSwarm.size() - 1] = new Individual(getElite()->params);

		return new Swarm(nextSwarm);
	}

	void print() {
		for(const auto i: swarm) i->print();
	}
};

int main() {
	std::chrono::system_clock::time_point start, end;
	/*
	start = std::chrono::system_clock::now();
	end = std::chrono::system_clock::now();
	double time = (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);
	printf("time: %lf\n", time);
	*/

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

