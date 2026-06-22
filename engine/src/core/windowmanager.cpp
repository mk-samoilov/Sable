#include "sableEng/core/windowmanager.h"

namespace Core
{
    void WindowManager::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Window = glfwCreateWindow(Width, Height, "Sable", nullptr, nullptr);
        glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
    }

    void WindowManager::CleanUp()
    {
        glfwDestroyWindow(Window);
        glfwTerminate();
    }

    void WindowManager::FramebufferResizeCallback(GLFWwindow*, int, int)
    {
        GetInstance()->SetResized(true);
    }
}
