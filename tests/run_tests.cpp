//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);
	system("pause");
	return result;
}