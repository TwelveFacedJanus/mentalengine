/**
 * @file CameraExample.cpp
 * @brief Implementation of CameraExample class
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 */

#include "CameraExample.h"
#include <cmath>

namespace MentalEngine {

CameraExample::CameraExample() {
    camera = std::make_shared<Camera>();
}

nil CameraExample::SetupFor3DModeling() {
    // Set up camera for 3D modeling
    camera->SetProjection(CameraProjection::Perspective);
    camera->SetFieldOfView(45.0f);
    camera->SetClippingPlanes(0.1f, 1000.0f);
    
    // Position camera for good 3D modeling view
    camera->SetPosition(Math::Vector3(5.0f, 5.0f, 5.0f));
    camera->SetTarget(Math::Vector3(0.0f, 0.0f, 0.0f));
    
    // Set orbit distance for comfortable modeling
    camera->SetOrbitDistance(8.0f);
}

nil CameraExample::SetupForTechnicalDrawing() {
    // Set up camera for technical drawing
    camera->SetProjection(CameraProjection::Orthographic);
    camera->SetOrthographicSize(10.0f);
    camera->SetClippingPlanes(0.1f, 1000.0f);
    
    // Position camera for technical drawing
    camera->SetPosition(Math::Vector3(0.0f, 0.0f, 10.0f));
    camera->SetTarget(Math::Vector3(0.0f, 0.0f, 0.0f));
    
    // Set orbit distance for technical drawing
    camera->SetOrbitDistance(10.0f);
}

nil CameraExample::SetupForArchitecturalViz() {
    // Set up camera for architectural visualization
    camera->SetProjection(CameraProjection::Perspective);
    camera->SetFieldOfView(60.0f); // Wider field of view for architecture
    camera->SetClippingPlanes(0.1f, 10000.0f); // Larger far plane for buildings
    
    // Position camera at human eye level
    camera->SetPosition(Math::Vector3(0.0f, 1.7f, 5.0f));
    camera->SetTarget(Math::Vector3(0.0f, 1.7f, 0.0f));
    
    // Set orbit distance for architectural scale
    camera->SetOrbitDistance(15.0f);
}

nil CameraExample::AnimateCamera(float time) {
    // Simple camera animation - circular orbit
    float radius = 10.0f;
    float speed = 0.5f;
    
    float x = radius * std::cos(time * speed);
    float z = radius * std::sin(time * speed);
    
    camera->SetPosition(Math::Vector3(x, 5.0f, z));
    camera->SetTarget(Math::Vector3(0.0f, 0.0f, 0.0f));
}

nil CameraExample::FitToObject(const Math::Vector3& object_center, const Math::Vector3& object_size) {
    // Calculate bounding box
    Math::Vector3 min_bounds = object_center - object_size * 0.5f;
    Math::Vector3 max_bounds = object_center + object_size * 0.5f;
    
    // Fit camera to show the object
    camera->FitToBounds(min_bounds, max_bounds);
}

} // namespace MentalEngine
