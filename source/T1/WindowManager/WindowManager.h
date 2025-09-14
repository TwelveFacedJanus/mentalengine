/**
 * @file WindowManager.h
 * @brief Window management system for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file defines the WindowManager template class, which provides a high-level
 * interface for managing application windows, OpenGL context, and coordinating
 * between the renderer and user interface systems.
 */

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "../../Core/Types.h"
#include "../Renderer/Renderer.h"
#include "source/T1/UserInterface/UserInterface.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "imgui.h"
#include "imgui_impl_glfw.h"

/**
 * @class WindowManager
 * @brief Template class for managing application windows and OpenGL context
 * 
 * The WindowManager class is responsible for creating and managing the main
 * application window, initializing OpenGL context, and coordinating between
 * the renderer and user interface systems. It provides a clean abstraction
 * over GLFW window management.
 * 
 * @tparam T Window type (defaults to GLFWwindow)
 * 
 * Key responsibilities:
 * - GLFW library initialization and cleanup
 * - Window creation and management
 * - OpenGL context setup
 * - Renderer and UI system coordination
 * - Main application loop execution
 * 
 * @note This class is non-copyable and non-movable for safety
 */
template <typename T = GLFWwindow>
class WindowManager
{
private:
    T* pWindow = nullptr;                                    ///< Pointer to the GLFW window
    std::shared_ptr<Renderer> pRenderer = nullptr;          ///< Shared pointer to the renderer
    std::shared_ptr<UserInterface<T>> pUI = nullptr;        ///< Shared pointer to the user interface

    /**
     * @brief Initializes the GLFW library
     * @private
     */
    nil __initialize_glfw_library() const;
    
    /**
     * @brief Sets OpenGL context hints for GLFW
     * @private
     */
    nil __set_glfw_hints() const;
    
    /**
     * @brief Initializes the renderer system
     * @private
     */
    nil __initialize_renderer();
    
    /**
     * @brief Creates the main application window
     * @private
     */
    nil __create_window();
    
    /**
     * @brief Loads and initializes the user interface
     * @private
     */
    nil __load_ui();

public:
    /**
     * @brief Constructor - initializes the window manager
     * 
     * Creates a new WindowManager instance and initializes all subsystems
     * including GLFW, OpenGL context, renderer, and user interface.
     */
    WindowManager();
    
    /**
     * @brief Destructor - cleans up resources
     * 
     * Destroys the window and terminates GLFW, cleaning up all resources.
     */
    ~WindowManager();
    
    /**
     * @brief Runs the main application loop
     * 
     * Executes the main application loop, handling events, rendering frames,
     * and updating the user interface until the window is closed.
     * 
     * @return nil This function does not return until the application exits
     */
    nil Run() const;
    
    /**
     * @brief Sets up input callbacks for camera control
     * @tparam T Window type
     * @private
     */
    nil __setup_input_callbacks();
    
    /**
     * @brief Deleted copy assignment operator
     * @param other Other WindowManager instance
     * @return WindowManager& Deleted
     */
    WindowManager operator=(WindowManager const&) = delete;
    
    /**
     * @brief Deleted copy constructor
     * @param other Other WindowManager instance
     */
    WindowManager(WindowManager const&) = delete;
};

/**
 * @brief Destructor implementation
 * @tparam T Window type
 */
template <typename T>
WindowManager<T>::~WindowManager() {
    if (this->pWindow) glfwDestroyWindow(this->pWindow);
    glfwTerminate();
}

/**
 * @brief Constructor implementation
 * @tparam T Window type
 * 
 * Initializes all subsystems in the correct order:
 * 1. GLFW library
 * 2. OpenGL hints
 * 3. Window creation
 * 4. Renderer initialization
 * 5. User interface loading
 */
template <typename T>
WindowManager<T>::WindowManager() {
    this->__initialize_glfw_library();
    this->__set_glfw_hints();
    this->__create_window();
    this->__initialize_renderer();
    this->__load_ui();
    this->__setup_input_callbacks();
}

/**
 * @brief Loads and initializes the user interface
 * @tparam T Window type
 * @private
 * 
 * Creates a new UserInterface instance with the window and renderer references.
 */
template <typename T>
nil WindowManager<T>::__load_ui() {
    pUI = std::make_shared<UserInterface<T>>(this->pWindow, this->pRenderer.get());
}

/**
 * @brief Runs the main application loop
 * @tparam T Window type
 * 
 * Main application loop that handles events, renders frames, and updates UI
 * until the window is closed by the user.
 */
template <typename T>
nil WindowManager<T>::Run() const {
    while (!glfwWindowShouldClose(this->pWindow)) {
        glfwPollEvents();
        pRenderer->DrawFrame([&]() { return pUI->DrawFrame(); });
        glfwSwapBuffers(this->pWindow);
    }
}

/**
 * @brief Sets OpenGL context hints for GLFW
 * @tparam T Window type
 * @private
 * 
 * Configures GLFW to use OpenGL 3.3 Core Profile for modern OpenGL features.
 */
template <typename T>
nil WindowManager<T>::__set_glfw_hints() const {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/**
 * @brief Initializes the renderer system
 * @tparam T Window type
 * @private
 * 
 * Creates a new Renderer instance and stores it in a shared pointer.
 */
template <typename T>
nil WindowManager<T>::__initialize_renderer() {
    this->pRenderer = std::make_shared<Renderer>();
}

/**
 * @brief Creates the main application window
 * @tparam T Window type
 * @private
 * 
 * Creates a new GLFW window with default settings and makes it the current
 * OpenGL context. Terminates GLFW if window creation fails.
 */
template <typename T>
nil WindowManager<T>::__create_window() {
    this->pWindow = glfwCreateWindow(800, 600, "Mental OpenGL", nullptr, nullptr);
    if (!this->pWindow) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->pWindow);
}

/**
 * @brief Initializes the GLFW library
 * @tparam T Window type
 * @private
 * 
 * Initializes the GLFW library. Returns early if initialization fails.
 */
template <typename T>
nil WindowManager<T>::__initialize_glfw_library() const {
    if (!glfwInit()) return;
}

/**
 * @brief Sets up input callbacks for camera control
 * @tparam T Window type
 * @private
 * 
 * Sets up GLFW input callbacks for mouse and keyboard input to control the camera.
 * This includes mouse button presses, mouse movement, scroll wheel, and keyboard input.
 * ImGui has priority over camera input - if ImGui wants to capture input, camera won't receive it.
 */
template <typename T>
nil WindowManager<T>::__setup_input_callbacks() {
    // Mouse button callback
    glfwSetMouseButtonCallback(this->pWindow, [](GLFWwindow* window, int button, int action, int mods) {
        (void)mods; // Suppress unused parameter warning
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
        if (!wm || !wm->pUI) return;
        
        // Проверяем, хочет ли ImGui захватить ввод
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            // Проверяем, находится ли мышь над viewport
            if (wm->pUI && wm->pUI->IsMouseOverViewport()) {
                // Мышь над viewport - передаем в камеру и в ImGui
                ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
                if (wm->pRenderer && wm->pRenderer->GetCamera()) {
                    double x, y;
                    glfwGetCursorPos(window, &x, &y);
                    wm->pRenderer->GetCamera()->HandleMouseButton(button, action, static_cast<float>(x), static_cast<float>(y));
                }
                // Также передаем в UserInterface для рисования
                if (wm->pUI) {
                    double x, y;
                    glfwGetCursorPos(window, &x, &y);
                    wm->pUI->HandleDrawingInput(button, action, static_cast<float>(x), static_cast<float>(y));
                }
            } else {
                // Мышь не над viewport - только ImGui
                ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
            }
            return;
        }
        
        // ImGui не хочет ввод, передаем в камеру
        if (wm->pRenderer && wm->pRenderer->GetCamera()) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            wm->pRenderer->GetCamera()->HandleMouseButton(button, action, static_cast<float>(x), static_cast<float>(y));
        }
    });
    
    // Mouse movement callback
    glfwSetCursorPosCallback(this->pWindow, [](GLFWwindow* window, double x, double y) {
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
        if (!wm || !wm->pUI) return;
        
        // Проверяем, хочет ли ImGui захватить ввод
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            // Проверяем, находится ли мышь над viewport
            if (wm->pUI && wm->pUI->IsMouseOverViewport()) {
                // Мышь над viewport - передаем в камеру и в ImGui
                ImGui_ImplGlfw_CursorPosCallback(window, x, y);
                if (wm->pRenderer && wm->pRenderer->GetCamera()) {
                    wm->pRenderer->GetCamera()->HandleMouseMove(static_cast<float>(x), static_cast<float>(y));
                }
                // Также передаем в UserInterface для рисования
                if (wm->pUI) {
                    wm->pUI->HandleDrawingMouseMove(static_cast<float>(x), static_cast<float>(y));
                }
            } else {
                // Мышь не над viewport - только ImGui
                ImGui_ImplGlfw_CursorPosCallback(window, x, y);
            }
            return;
        }
        
        // ImGui не хочет ввод, передаем в камеру
        if (wm->pRenderer && wm->pRenderer->GetCamera()) {
            wm->pRenderer->GetCamera()->HandleMouseMove(static_cast<float>(x), static_cast<float>(y));
        }
    });
    
    // Mouse scroll callback
    glfwSetScrollCallback(this->pWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
        if (!wm || !wm->pUI) return;
        
        // Проверяем, хочет ли ImGui захватить ввод
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            // Проверяем, находится ли мышь над viewport
            if (wm->pUI && wm->pUI->IsMouseOverViewport()) {
                // Мышь над viewport - передаем в камеру, но также передаем в ImGui
                ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
                if (wm->pRenderer && wm->pRenderer->GetCamera()) {
                    wm->pRenderer->GetCamera()->HandleMouseScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
                }
            } else {
                // Мышь не над viewport - только ImGui
                ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
            }
            return;
        }
        
        // ImGui не хочет ввод, передаем в камеру
        if (wm->pRenderer && wm->pRenderer->GetCamera()) {
            wm->pRenderer->GetCamera()->HandleMouseScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
        }
    });
    
    // Keyboard callback
    glfwSetKeyCallback(this->pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        (void)scancode; // Suppress unused parameter warning
        (void)mods; // Suppress unused parameter warning
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
        if (!wm || !wm->pUI) return;
        
        // Проверяем, хочет ли ImGui захватить ввод
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            // Проверяем, находится ли мышь над viewport
            if (wm->pUI && wm->pUI->IsMouseOverViewport()) {
                // Мышь над viewport - передаем в камеру, но также передаем в ImGui
                ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
                if (wm->pRenderer && wm->pRenderer->GetCamera()) {
                    wm->pRenderer->GetCamera()->HandleKey(key, action);
                }
            } else {
                // Мышь не над viewport - только ImGui
                ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
            }
            return;
        }
        
        // ImGui не хочет ввод, передаем в камеру
        if (wm->pRenderer && wm->pRenderer->GetCamera()) {
            wm->pRenderer->GetCamera()->HandleKey(key, action);
        }
    });
    
    // Character callback for text input (needed for ImGui text input)
    glfwSetCharCallback(this->pWindow, [](GLFWwindow* window, unsigned int codepoint) {
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
        if (!wm || !wm->pUI) return;
        
        // Проверяем, хочет ли ImGui захватить ввод
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantTextInput) {
            // ImGui хочет захватить ввод, передаем ему событие
            ImGui_ImplGlfw_CharCallback(window, codepoint);
        }
    });
    
    // Set window user pointer for callbacks
    glfwSetWindowUserPointer(this->pWindow, this);
}

#endif // WINDOW_MANAGER_H
