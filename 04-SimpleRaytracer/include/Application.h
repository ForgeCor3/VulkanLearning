#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"
#include <stdexcept>


class Application final
{
private:
    Window window;

public:
    Application() = delete;
    Application(const WindowConfig& windowConfig);

    ~Application() { };

    void run();
    void mainloop();
    void terminate();
};

#endif // APPLICATION_H