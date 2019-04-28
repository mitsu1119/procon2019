#pragma once

#include <ctime>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <vector>
#include <algorithm>

// 境界を二分探索するやつ
// ソートされたvector Aの内, A[i] < b となる最大のインデックスiを返す
template <typename T>
size_t boundarySearch(std::vector<T> A, T b) {
	int_fast32_t left = -1;
	int_fast32_t right = (int)A.size();
	int_fast32_t mid;

	while(right - left > 1) {
		mid = left + (right - left) / 2;
		if(A[mid] >= b) right = mid;
		else left = mid;
	}
	return left;
}

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

	// ヒストグラム生成
	void histogram() {
		// 現在のデータをファイル書き出し
		FILE *fp = fopen("data.txt", "w");
		for(uint_fast32_t i = 0; i < this->data.size(); i++) fprintf(fp, "%s\n", std::to_string(this->data[i]).c_str());
		fclose(fp);

		// gnuplot
		FILE *gp = popen("python", "w");

		fprintf(gp, "import numpy as np\nfrom matplotlib import pyplot\n");
		fprintf(gp, "data = np.loadtxt('data.txt')\n");
		fprintf(gp, "pyplot.hist(data, bins=%u)\n", (uint_fast32_t)(this->data.size()/100));
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
DECLARE_ENUM(Direction, direction2name, UP, RUP, RIGHT, RDOWN, DOWN, LDOWN, LEFT, LUP, DIRECTION_SIZE);

// Direction型からx,yを所得
int_fast32_t direction2x(Direction direction);
int_fast32_t direction2y(Direction direction);

// 乱数生成器
// 高速で性質がいいため128bitのXoroshiro+法を選択
// Xorshift familyに比べていい乱数がでるし高速
class XorOshiro128p {
private:
	uint_fast64_t seed[2];

	uint_fast64_t gen();

public:
	XorOshiro128p();
	XorOshiro128p(uint_fast64_t seed);

	// uint_fast64_t の範囲で乱数を生成
	uint_fast64_t randull();

	// [0, max]の範囲で乱数を生成
	uint_fast64_t randull(uint_fast64_t max);

	uint_fast64_t operator()() {
		return gen();
	}
};

	
