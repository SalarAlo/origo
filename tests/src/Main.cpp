#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "origo/core/Logger.h"

int main(int argc, char** argv) {
	Origo::Logger::Init();

	doctest::Context context;
	context.applyCommandLine(argc, argv);
	const int result = context.run();

	Origo::Logger::Shutdown();
	return result;
}
