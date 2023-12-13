#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "WindowConfig.h"
#include "GlobUtils.h"

class Window final
{
public:
	DISABLE_COPY_AND_MOVE(Window)

	Window() = delete;
	Window(const WindowConfig windowConfig);
	~Window();

	GLFWwindow*				 getWindow() const;
	std::vector<const char*> getRequiredInstanceExtensions() const;

private:
	GLFWwindow*	 window;
	WindowConfig windowConfig;
};

#endif // WINDOW_H