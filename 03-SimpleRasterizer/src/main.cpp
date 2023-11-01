#include "SimpleRasterizer.h"

int main()
{
	SimpleRasterizer app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}