#include "Application.h"

int main()
{
    const WindowConfig windowConfig = { "Test", 800, 600, false, true };

    Application application(windowConfig);
    application.run();
}