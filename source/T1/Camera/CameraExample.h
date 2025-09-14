/**
 * @file CameraExample.h
 * @brief Example usage of the Camera system
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file demonstrates how to use the Camera system for CAD-style navigation.
 */

#ifndef MENTAL_CAMERA_EXAMPLE_H
#define MENTAL_CAMERA_EXAMPLE_H

#include "Camera.h"
#include "../../Core/Math.h"

namespace MentalEngine {

/**
 * @class CameraExample
 * @brief Example class demonstrating camera usage
 * 
 * This class shows how to set up and use the camera system
 * for typical CAD-style applications.
 */
class CameraExample {
private:
    std::shared_ptr<Camera> camera;
    
public:
    /**
     * @brief Constructor
     */
    CameraExample();
    
    /**
     * @brief Destructor
     */
    ~CameraExample() = default;
    
    /**
     * @brief Gets the camera instance
     * @return std::shared_ptr<Camera> Camera instance
     */
    std::shared_ptr<Camera> GetCamera() const { return camera; }
    
    /**
     * @brief Sets up camera for 3D modeling
     */
    nil SetupFor3DModeling();
    
    /**
     * @brief Sets up camera for technical drawing
     */
    nil SetupForTechnicalDrawing();
    
    /**
     * @brief Sets up camera for architectural visualization
     */
    nil SetupForArchitecturalViz();
    
    /**
     * @brief Demonstrates camera animation
     * @param time Current time in seconds
     */
    nil AnimateCamera(float time);
    
    /**
     * @brief Fits camera to show a specific object
     * @param object_center Object center position
     * @param object_size Object bounding box size
     */
    nil FitToObject(const Math::Vector3& object_center, const Math::Vector3& object_size);
};

} // namespace MentalEngine

#endif // MENTAL_CAMERA_EXAMPLE_H
