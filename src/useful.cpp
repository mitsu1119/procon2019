#include "useful.hpp"

UF::UF(uint_fast32_t size) {
	data = std::vector<int_fast32_t>(size);
	rank = std::vector<int_fast32_t>(size);
	for(size_t i = 0; i < size; i++) {
		data[i] = i;
		rank[i] = 0;
	}
}

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
  	case STOP:
	  	return 0;									
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
  	case STOP:
	  	return 0;
	}
	return 0;
}

Direction xyToDirection(int_fast32_t dx, int_fast32_t dy) {
	int x = (dx + (7 * dy + 7) / (3 * dx + 4) + 8) % 8;
	int y = 4 - dx;
	int dir = (((dy + 1) / 2 + 1) % 2) * x + (dy + 1) / 2 * y;
	return (Direction)dir;
}

XorOshiro128p::XorOshiro128p() {
}

XorOshiro128p::XorOshiro128p(uint_fast64_t seed) {
	if(seed == 0) seed = time(NULL);

	this->seed[0] = splitmix64(seed);
	this->seed[1] = splitmix64(this->seed[0]);
}

