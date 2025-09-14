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

#endif // WINDOW_MANAGER_H
