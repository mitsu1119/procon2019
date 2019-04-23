#pragma once

#include <ctime>
#include <cstdint>
#include <random>

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

	
