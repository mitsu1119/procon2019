// make tester
// ./tester
// --------------------------
// box1
// 0xXXXXXX
// box2
// 0xYYYYYY
// change box1: 0xYYYYYY
// box2
// change box2: 0xXXXXXX
// box1
// --------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Box Box;
struct Box {
	char *name;
};

void printBox(Box *box) {
	printf("%s\n", box->name);
}

void freeBox(Box *box) {
	delete box->name;
	delete box;
}

int main() {
	Box *box1 = new Box;
	box1->name = strdup("box1");

	Box *box2 = new Box;
	box2->name = strdup("box2");

	printBox(box1);
	printf("%p\n", box1);
	printBox(box2);
	printf("%p\n", box2);

	box1 = (Box *)((unsigned long)box1 ^ (unsigned long)box2);
	box2 = (Box *)((unsigned long)box2 ^ (unsigned long)box1);
	box1 = (Box *)((unsigned long)box1 ^ (unsigned long)box2);
	printf("change box1: %p\n", box1);
	printBox(box1);
	printf("change box2: %p\n", box2);
	printBox(box2);

	freeBox(box1);
	freeBox(box2);

	return 0;
}
