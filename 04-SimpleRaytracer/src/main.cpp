#include "Application.h"

#ifdef NDEBUG
	#define DEBUG_MODE false
#else
	#define DEBUG_MODE true
#endif

int main()
{
	const WindowConfig windowConfig = { "SimpleRaytracer", 800, 600, true, false };

	Application application(windowConfig, DEBUG_MODE);
	application.run();
}