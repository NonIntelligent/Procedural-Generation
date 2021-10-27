#include "Generator.h"

#include <memory>

int main(int argc, char* args[]) {
	std::unique_ptr<Generator> generator = std::make_unique<Generator>();

	if(!generator->init()) return -1;

	generator->mainLoop();

	return 0;
}
