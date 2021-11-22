#include "Procedural.h"

#include <memory>

int main(int argc, char* args[]) {
	std::unique_ptr<Procedural> procedural = std::make_unique<Procedural>();

	if(!procedural->init()) return -1;

	procedural->printMemoryUsage();

	procedural->mainLoop();

	return 0;
}
