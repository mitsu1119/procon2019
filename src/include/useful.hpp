#pragma once

#include <ctime>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

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

	
