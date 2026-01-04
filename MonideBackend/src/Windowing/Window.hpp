#ifdef X11
#define GLFW_EXPOSE_NATIVE_X11
#include <X11/Xlib.h>

#elif defined WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <wayland-client.h>

#elif defined _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windef.h>
#include <GLFW/GLFW3Native.h>
#endif

/* screw apple bruh
#define GLFW_EXPOSE_NATIVE_COCOA
#include <Cocoa/Cocoa.h>
#endif
*/

#include <GLFW/GLFW3.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace Monide
{
    // Memory Layout [glfwWindow: 8 bytes] [width: 2] [height: 2] [padding: 4] [title: 8 bytes]
    struct Window
    {
        GLFWwindow *glfwWindow;
        uint16_t width, height; // 65536 pixels max (but who has that massive of a screen)
        const char *title;

        Window(uint16_t width, uint16_t height, const char *title)
            : glfwWindow(nullptr), width(width), height(height), title(title)
        {
            if (!glfwInit())
            {
                throw std::runtime_error("GLFW was not initialized before attempting to create a window.");
            }
            glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
            if (!glfwWindow)
            {
                throw std::runtime_error("Failed to create GLFW window.");
            }
        }

        ~Window()
        {
            if (glfwWindow)
            {
                glfwDestroyWindow(glfwWindow);
            }
        }
        uint16_t GetWidth() const { return width; }
        uint16_t GetHeight() const { return height; }
        std::pair<uint16_t, uint16_t> GetMonitorDimentions() const
        {
            GLFWmonitor *monitor = glfwGetWindowMonitor(glfwWindow);
            if (!monitor)
            {
                throw std::runtime_error("Failed to get primary monitor.");
            }
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (!mode)
            {
                throw std::runtime_error("Failed to get video mode of primary monitor.");
            }
            return std::make_pair(mode->width, mode->height);
        }

        void MaximizeHack()
        {
            auto [monWidth, monHeight] = GetMonitorDimentions();
            glfwSetWindowSize(glfwWindow, monWidth, monHeight);
            width = static_cast<uint16_t>(monWidth);
            height = static_cast<uint16_t>(monHeight);
        }
        void MaximizeWindow()
        {
            glfwMaximizeWindow(glfwWindow);
        }
        void MinimizeWindow()
        {
#if defined(_WIN32)
            HWND hwnd = glfwGetWin32Window(glfwWindow);
            ShowWindow(hwnd, SW_MINIMIZE);

#elif defined(__linux__)

#if defined(GLFW_EXPOSE_NATIVE_X11)

            Display *display = glfwGetX11Display();
            ::Window x11Window = glfwGetX11Window(glfwWindow);

            Atom wm_change_state = XInternAtom(display, "WM_CHANGE_STATE", False);
            Atom wm_state = XInternAtom(display, "WM_STATE", False);

            XEvent event{};
            event.type = ClientMessage;
            event.xclient.window = x11Window;
            event.xclient.message_type = wm_change_state;
            event.xclient.format = 32;
            event.xclient.data.l[0] = IconicState;

            XSendEvent(display,
                       DefaultRootWindow(display),
                       False,
                       SubstructureNotifyMask | SubstructureRedirectMask,
                       &event);

            XFlush(display);

#elif defined(GLFW_EXPOSE_NATIVE_WAYLAND)

            // Wayland has no app-driven minimize.
            // Correct behavior is to unmap the surface.
            glfwHideWindow(glfwWindow);
            glfwIconifyWindow(glfwWindow);

#endif

#else
            // Other platforms (BSD, etc.)
            glfwHideWindow(glfwWindow); // AHHHHHHHHHH
            glfwIconifyWindow(glfwWindow);
#endif
        }
    };

    class WindowManager
    {
    public:
        static std::vector<Window *> windows;
        static bool GlobalInit()
        {
            if (!glfwInit())
            {
                throw std::runtime_error("GLFW failed to initialize.");
                return false;
            }
            return true;
        }
        static bool GlobalShutdown()
        {
            for (Window *win : windows)
            {
                glfwDestroyWindow(win->glfwWindow);
                delete win;
            }
            glfwTerminate();
            return true;
        }
    };
}