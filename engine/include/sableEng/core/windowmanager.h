#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "sableEng/utils/defines.h"

namespace Core
{
    class WindowManager : public Utils::Singleton<WindowManager>
    {
        public:
            void InitWindow();
            void CleanUp();

            bool ShouldClose() const { return glfwWindowShouldClose(Window); }
            void PollEvents() const { glfwPollEvents(); }
            void WaitEvents() const { glfwWaitEvents(); }

            GLFWwindow* GetWindow() const { return Window; }
            void GetFramebufferSize(int* width, int* height) const { glfwGetFramebufferSize(Window, width, height); }

            bool IsResized() const { return FramebufferResized; }
            void SetResized(bool resized) { FramebufferResized = resized; }

            void CloseWindow() { glfwSetWindowShouldClose(Window, GLFW_TRUE); }

        private:
            static void FramebufferResizeCallback(GLFWwindow*, int, int);

            GLFWwindow* Window = nullptr;
            bool FramebufferResized = false;

            int Width = 800;
            int Height = 600;
    };
}
