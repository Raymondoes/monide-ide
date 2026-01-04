#pragma once

#include <cstdlib>
#include "../Windowing/Window.hpp"

namespace Monide
{
    void CloseMonide()
    {
        if (!WindowManager::GlobalShutdown())
            glfwTerminate();
        exit(0);
    }

}
