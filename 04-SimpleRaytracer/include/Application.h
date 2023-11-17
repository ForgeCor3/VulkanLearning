#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "Window.h"
#include "Vulkan/VulkanInstance.h"

class Application final
{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<VulkanInstance> instance;

public:
    Application() = delete;
    Application(const WindowConfig& windowConfig);

    ~Application() { };

    void run();
    void mainloop();
    void terminate();
};

#endif // APPLICATION_H