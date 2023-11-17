#include "Application.h"

int main()
{
    const WindowConfig windowConfig = { "Test", 800, 600, true, false };

    Application application(windowConfig);
    application.run();
}