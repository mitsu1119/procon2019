#include "include/useful.hpp"

static inline uint_fast64_t rotl(uint_fast64_t x, int_fast32_t k) {
	return (x << k) | (x >> (std::numeric_limits<uint_fast64_t>::digits - k));
}

static inline uint_fast64_t splitmix64(uint_fast64_t z) {
	z += 0x9e3779b97f4a7c15;
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

XorOshiro128p::XorOshiro128p() {
}

XorOshiro128p::XorOshiro128p(uint_fast64_t seed) {
	if(seed == 0) seed = time(NULL);

	this->seed[0] = splitmix64(seed);
	this->seed[1] = splitmix64(this->seed[0]);
}

uint_fast64_t XorOshiro128p::randull() {
	return gen();
}

uint_fast64_t XorOshiro128p::randull(uint_fast64_t max) {
	// 厳密にはこうするべきだがパフォーマンスが落ちる
	// return std::uniform_int_distribution<uint_fast64_t>{0, max-1}(*this);
	
	return gen() % (max + 1);
}

uint_fast64_t XorOshiro128p::gen() {
	uint_fast64_t s0 = this->seed[0], s1 = this->seed[1], result = s0 + s1;
	s1 ^= s0;
	this->seed[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
	this->seed[1] = rotl(s1, 36);

	return result;
}
