/**
 * @file main.cpp
 * @brief Main entry point for MentalEngine application
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file contains the main function that serves as the entry point for the
 * MentalEngine application. It initializes the T1 layer and starts the main
 * application loop.
 */

#include "T1/T1.h"

/**
 * @brief Main entry point of the MentalEngine application
 * 
 * This function creates an instance of the MentalT1Layer class and starts
 * the main application loop. The T1 layer handles all the core functionality
 * including window management, rendering, and user interface.
 * 
 * @return int Exit status code (0 for success)
 * 
 * @note This function will run until the user closes the application window
 *       or the application is terminated by other means.
 */
int main() {
    MentalT1Layer t1;
    t1.Run();
}
