/**
 * @file T1.h
 * @brief T1 Layer - Main application layer for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file defines the MentalT1Layer class, which serves as the main application
 * layer in the MentalEngine architecture. The T1 layer acts as a high-level
 * abstraction that coordinates between different subsystems including window
 * management, rendering, and user interface.
 */

#ifndef MENTAL_T1_LAYER_H
#define MENTAL_T1_LAYER_H

#include "WindowManager/WindowManager.h"
#include <memory>
#include <GLFW/glfw3.h>
#include "../Core/Types.h"

/**
 * @class MentalT1Layer
 * @brief Main application layer that coordinates all engine subsystems
 * 
 * The MentalT1Layer class serves as the top-level coordinator for the MentalEngine
 * application. It manages the window manager and provides a simple interface
 * to start and run the entire application.
 * 
 * This class follows the singleton pattern and is responsible for:
 * - Initializing the window management system
 * - Coordinating between different engine subsystems
 * - Providing the main application loop
 * 
 * @note This class is designed to be the primary entry point for the engine
 *       and should be instantiated once per application.
 */
class MentalT1Layer {
private:
    std::shared_ptr<class WindowManager<GLFWwindow>> ptrWindowManager = std::make_shared<WindowManager<GLFWwindow>>(); ///< Shared pointer to the window manager

public:
    /**
     * @brief Default constructor
     * 
     * Creates a new instance of MentalT1Layer. The window manager is automatically
     * initialized with default settings.
     */
    MentalT1Layer() {}
    
    /**
     * @brief Starts the main application loop
     * 
     * This method initiates the main application loop by delegating to the
     * window manager's Run() method. The application will continue running
     * until the user closes the window or the application is terminated.
     * 
     * @return nil This function does not return until the application exits
     * 
     * @note This is a blocking call that will run the main application loop
     */
    inline nil Run() const { this->ptrWindowManager->Run(); }
    
    /**
     * @brief Destructor
     * 
     * Cleans up resources when the MentalT1Layer instance is destroyed.
     * The window manager will be automatically cleaned up through the
     * shared_ptr mechanism.
     */
    ~MentalT1Layer() {}
};

#endif // MENTAL_T1_LAYER_H
