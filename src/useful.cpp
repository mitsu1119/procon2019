#include "useful.hpp"

int_fast32_t direction2x(Direction direction) {
	switch(direction) {
		case UP:
		case DOWN:
			return 0;
		case RUP:
		case RIGHT:
		case RDOWN:
			return 1;
		case LDOWN:
		case LEFT:
		case LUP:
			return -1;
	}
	return 0;
}

int_fast32_t direction2y(Direction direction) {
	switch(direction) {
		case UP:
		case RUP:
		case LUP:
			return -1;
		case RIGHT:
		case LEFT:
			return 0;
		case RDOWN:
		case DOWN:
		case LDOWN:
			return 1;
	}
	return 0;
}

XorOshiro128p::XorOshiro128p() {
}

XorOshiro128p::XorOshiro128p(uint_fast64_t seed) {
	if(seed == 0) seed = time(NULL);

	this->seed[0] = splitmix64(seed);
	this->seed[1] = splitmix64(this->seed[0]);
}

