#pragma once

#include <ctime>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <vector>
#include <algorithm>

// UnionFind クラス
class UF {
private:
	std::vector<int_fast32_t> rank;
	std::vector<int_fast32_t> data;

public:
	UF(uint_fast32_t size);

	inline int_fast32_t root(int_fast32_t x) {
		return data[x] == x ? x : data[x] = root(data[x]);
	}

	inline bool same(int_fast32_t x, int_fast32_t y) {
		return root(x) == root(y);
	}

	inline void unite(int_fast32_t x, int_fast32_t y) {
		x = root(x);
		y = root(y);
		if(x == y) return;

		if(rank[x] < rank[y]) {
			data[x] = y;
		} else {
			data[y] = x;
			if(rank[x] == rank[y]) rank[x]++;
		}
	}
};

// グラフ表示するやつ
// template使うのでヘッダに実装
template <typename T>
class DPlot {
private:
	// データ
	std::vector<T> data;

public:
	DPlot(std::vector<T> data):data(data) {
	}

	T calcMean();

	// ヒストグラム生成
	void histogram(std::string color = "c") {
		// 現在のデータをファイル書き出し
		FILE *fp = fopen("data.txt", "w");
		for(uint_fast32_t i = 0; i < this->data.size(); i++) fprintf(fp, "%s\n", std::to_string(this->data[i]).c_str());
		fclose(fp);

		// gnuplot
		FILE *gp = popen("python", "w");

		fprintf(gp, "import numpy as np\nfrom matplotlib import pyplot\n");
		fprintf(gp, "data = np.loadtxt('data.txt')\n");
		fprintf(gp, "pyplot.hist(data, bins=%u, color='%s')\n", (uint_fast32_t)(this->data.size()/100), color.c_str());
		fprintf(gp, "pyplot.show()\n");
		pclose(gp);

		std::remove("data.txt");
	}
};

class enumName {
private:
	std::vector<std::string> args;
	std::string buf = "";

	void argSplit(std::string str) {	// split ','
		char charactor;
		int currentPos = 0;
		while((unsigned int)currentPos < str.size()) {
			charactor = str[currentPos++];
			if(std::isspace(charactor)) continue;
			if(currentPos == str.size()) {
				buf += charactor;
				args.emplace_back(buf);
				break;
			}
			if(charactor != ',') buf += charactor;
			else {
				args.emplace_back(buf);
				buf = "";
			}
		}
	}

public:
	enumName(std::string enums) {
		argSplit(enums);
	}
	std::string operator[](int i) {
		return args[i];
	}
};

#define DECLARE_ENUM(enumname, enumstr, ...) \
enum enumname { __VA_ARGS__ }; \
static enumName enumstr(#__VA_ARGS__); 

// Direction型
// direction2name[UP] = "UP" など
DECLARE_ENUM(Direction, direction2name, UP, RUP, RIGHT, RDOWN, DOWN, LDOWN, LEFT, LUP, STOP ,NONE, DIRECTION_SIZE);

// Direction型からx,yを所得
int_fast32_t direction2x(Direction direction);
int_fast32_t direction2y(Direction direction);

// 乱数生成器
// 高速で性質がいいため128bitのXoroshiro+法を選択
// Xorshift に比べていい乱数がでるし高速
// 64bit
class XorOshiro128p {
private:
	uint_fast64_t seed[2];

	static uint_fast64_t rotl(uint_fast64_t x, int_fast32_t k);

	static uint_fast64_t splitmix64(uint_fast64_t z);

public:
	XorOshiro128p();
	XorOshiro128p(uint_fast64_t seed);

	// それぞれ同じ処理を実装しているのは高速化のため(関数呼び出しのコストが大きい)
	
	uint_fast64_t operator()() {
		uint_fast64_t s0 = this->seed[0], s1 = this->seed[1], result = s0 + s1;
		s1 ^= s0; 
		this->seed[0] = ((s0 << 55) | (s0 >> (64 - 55))) ^ s1 ^ (s1 << 14);
		this->seed[1] = ((s1 << 36) | (s1 >> (64 - 36)));
		return result;
	}

	uint_fast64_t operator()(uint_fast64_t max) {
		uint_fast64_t s0 = this->seed[0], s1 = this->seed[1], result = s0 + s1;
		s1 ^= s0; 
		this->seed[0] = ((s0 << 55) | (s0 >> (64 - 55))) ^ s1 ^ (s1 << 14);
		this->seed[1] = ((s1 << 36) | (s1 >> (64 - 36)));

		// modulo bias がかかってしまうがパフォーマンスを考えると難しい
		return result % (max + 1);
	}

	// [0, 1) の範囲で乱数を生成する
	inline double gend() {
		uint_fast64_t s0 = this->seed[0], s1 = this->seed[1], result = s0 + s1;
		s1 ^= s0; 
		this->seed[0] = ((s0 << 55) | (s0 >> (64 - 55))) ^ s1 ^ (s1 << 14);
		this->seed[1] = ((s1 << 36) | (s1 >> (64 - 36)));
		return (result >> 11) * (1. / (UINT64_C(1) << 53));
	}

	inline bool genb() {
		return (this->gend() < 0.5) ? true : false;
	}
};
	
inline uint_fast64_t XorOshiro128p::splitmix64(uint_fast64_t z) {
	z += 0x9e3779b97f4a7c15;
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}
