#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "../../Core/Types.h"
#include "../Renderer/Renderer.h"
#include "source/T1/UserInterface/UserInterface.h"
#include <GLFW/glfw3.h>
#include <memory>

template <typename T = GLFWwindow>
class WindowManager
{
private:
    T* pWindow = nullptr;
    std::shared_ptr<Renderer> pRenderer = nullptr;
    std::shared_ptr<UserInterface<T>> pUI = nullptr;

    nil __initialize_glfw_library() const;
    nil __set_glfw_hints() const;
    nil __initialize_renderer();
    nil __create_window();
    nil __load_ui();
public:
    WindowManager();
    ~WindowManager();
    nil Run() const;
    WindowManager operator=(WindowManager const&) = delete;
    WindowManager(WindowManager const&) = delete;
};

template <typename T>
WindowManager<T>::~WindowManager() {
    if (this->pWindow) glfwDestroyWindow(this->pWindow);
    glfwTerminate();
}

template <typename T>
WindowManager<T>::WindowManager() {
    this->__initialize_glfw_library();
    this->__set_glfw_hints();
    this->__create_window();
    this->__initialize_renderer();
    this->__load_ui();
}

template <typename T>
nil WindowManager<T>::__load_ui() {
    pUI = std::make_shared<UserInterface<T>>(this->pWindow);
}

template <typename T>
nil WindowManager<T>::Run() const {
    while (!glfwWindowShouldClose(this->pWindow)) {
        glfwPollEvents();
        pRenderer->DrawFrame([&]() { return pUI->DrawFrame(); });
        glfwSwapBuffers(this->pWindow);
    }
}

template <typename T>
nil WindowManager<T>::__set_glfw_hints() const {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

template <typename T>
nil WindowManager<T>::__initialize_renderer() {
    this->pRenderer = std::make_shared<Renderer>();
}

template <typename T>
nil WindowManager<T>::__create_window() {
    this->pWindow = glfwCreateWindow(800, 600, "Mental OpenGL", nullptr, nullptr);
    if (!this->pWindow) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->pWindow);
}

template <typename T>
nil WindowManager<T>::__initialize_glfw_library() const {
    if (!glfwInit()) return;
}

#endif // WINDOW_MANAGER_H
