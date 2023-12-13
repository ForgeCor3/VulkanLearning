#ifndef WINDOWCONFIG_H
#define WINDOWCONFIG_H

#include <string>
#include <cstdint>

struct WindowConfig final
{
	std::string windowTitle;
	uint16_t	windowWidth;
	uint16_t	windowHeight;
	bool		isResizable;
	bool		isFullscreen;

	WindowConfig& operator=(const WindowConfig& windowConfig)
	{
		if (this != &windowConfig)
		{
			windowTitle = windowConfig.windowTitle;
			windowWidth = windowConfig.windowWidth;
			windowHeight = windowConfig.windowHeight;
			isResizable = windowConfig.isResizable;
			isFullscreen = windowConfig.isFullscreen;
		}

		return *this;
	}
};

#endif // WINDOWCONFIG_H