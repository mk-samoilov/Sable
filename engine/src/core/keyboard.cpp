#include "sableEng/core/keyboard.h"
#include "sableEng/core/windowmanager.h"

#include <GLFW/glfw3.h>

namespace Core
{
    void Keyboard::Update()
    {
        GLFWwindow* window = Core::WindowManager::GetInstance()->GetWindow();

        for (auto b : Binds) {
            if (glfwGetKey(window, b.Key) == GLFW_PRESS) b.Func(); 
        }
    }
}
