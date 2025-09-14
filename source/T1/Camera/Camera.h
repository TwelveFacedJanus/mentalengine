/**
 * @file Camera.h
 * @brief Camera system for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file defines the Camera class, which provides CAD-style camera controls
 * including pan, zoom, and orbit functionality.
 */

#ifndef MENTAL_CAMERA_H
#define MENTAL_CAMERA_H

#include "../../Core/Types.h"
#include "../../Core/Math.h"
#include <GLFW/glfw3.h>

namespace MentalEngine {

/**
 * @enum CameraProjection
 * @brief Camera projection types
 */
enum class CameraProjection {
    Perspective,
    Orthographic
};

/**
 * @enum CameraMode
 * @brief Camera interaction modes
 */
enum class CameraMode {
    Orbit,      // Orbit around target point
    Pan,        // Pan the camera
    Zoom        // Zoom in/out
};

/**
 * @class Camera
 * @brief CAD-style camera system
 * 
 * The Camera class provides comprehensive camera functionality similar to
 * standard CAD applications. It supports both perspective and orthographic
 * projections, and includes intuitive controls for navigation.
 * 
 * Key features:
 * - Orbit around target point
 * - Pan and zoom functionality
 * - Perspective and orthographic projections
 * - Smooth camera transitions
 * - Configurable field of view and clipping planes
 */
class Camera {
private:
    // Camera properties
    Math::Vector3 position;           ///< Camera position in world space
    Math::Vector3 target;             ///< Target point to look at
    Math::Vector3 up;                 ///< Up vector
    CameraProjection projection;      ///< Current projection type
    
    // Projection parameters
    float fov;                        ///< Field of view (for perspective)
    float aspect_ratio;               ///< Aspect ratio
    float near_plane;                 ///< Near clipping plane
    float far_plane;                  ///< Far clipping plane
    
    // Orthographic parameters
    float ortho_size;                 ///< Orthographic view size
    
    // Camera control state
    bool is_rotating;                 ///< Currently rotating camera
    bool is_panning;                  ///< Currently panning camera
    bool is_zooming;                  ///< Currently zooming camera
    bool is_ctrl_pressed;             ///< Ctrl key is currently pressed
    Math::Vector2 last_mouse_pos;     ///< Last mouse position
    Math::Vector2 mouse_delta;        ///< Mouse movement delta
    
    // Orbit parameters
    float orbit_distance;             ///< Distance from target
    float orbit_angle_x;              ///< Horizontal orbit angle
    float orbit_angle_y;              ///< Vertical orbit angle
    float min_orbit_distance;         ///< Minimum orbit distance
    float max_orbit_distance;         ///< Maximum orbit distance
    
    // Pan parameters
    Math::Vector3 pan_speed;          ///< Pan movement speed
    
    // Zoom parameters
    float zoom_speed;                 ///< Zoom speed
    float zoom_factor;                ///< Current zoom factor
    
    /**
     * @brief Updates camera position based on orbit parameters
     * @private
     */
    nil __update_orbit_position();
    
    /**
     * @brief Constrains orbit angles to valid ranges
     * @private
     */
    nil __constrain_orbit_angles();
    
    /**
     * @brief Updates aspect ratio based on viewport size
     * @private
     */
    nil __update_aspect_ratio(int width, int height);

public:
    /**
     * @brief Constructor
     * 
     * Initializes the camera with default settings suitable for CAD applications.
     * Sets up perspective projection with reasonable default values.
     */
    Camera();
    
    /**
     * @brief Destructor
     */
    ~Camera() = default;
    
    // Projection methods
    /**
     * @brief Sets the projection type
     * @param proj Projection type (Perspective or Orthographic)
     */
    nil SetProjection(CameraProjection proj);
    
    /**
     * @brief Gets the current projection type
     * @return CameraProjection Current projection type
     */
    CameraProjection GetProjection() const { return projection; }
    
    /**
     * @brief Sets field of view for perspective projection
     * @param fov Field of view in degrees
     */
    nil SetFieldOfView(float fov);
    
    /**
     * @brief Sets orthographic view size
     * @param size Orthographic view size
     */
    nil SetOrthographicSize(float size);
    
    /**
     * @brief Sets clipping planes
     * @param near_plane Near clipping plane distance
     * @param far_plane Far clipping plane distance
     */
    nil SetClippingPlanes(float near_plane, float far_plane);
    
    // Camera positioning
    /**
     * @brief Sets camera position
     * @param pos New camera position
     */
    nil SetPosition(const Math::Vector3& pos);
    
    /**
     * @brief Sets target point
     * @param target New target point
     */
    nil SetTarget(const Math::Vector3& target);
    
    /**
     * @brief Gets camera position
     * @return Math::Vector3 Current camera position
     */
    Math::Vector3 GetPosition() const { return position; }
    
    /**
     * @brief Gets target point
     * @return Math::Vector3 Current target point
     */
    Math::Vector3 GetTarget() const { return target; }
    
    // Orbit controls
    /**
     * @brief Sets orbit distance
     * @param distance Distance from target
     */
    nil SetOrbitDistance(float distance);
    
    /**
     * @brief Sets orbit angles
     * @param angle_x Horizontal angle in degrees
     * @param angle_y Vertical angle in degrees
     */
    nil SetOrbitAngles(float angle_x, float angle_y);
    
    /**
     * @brief Orbits around target point
     * @param delta_x Horizontal rotation delta
     * @param delta_y Vertical rotation delta
     */
    nil Orbit(float delta_x, float delta_y);
    
    // Pan controls
    /**
     * @brief Pans the camera
     * @param delta_x Horizontal pan delta
     * @param delta_y Vertical pan delta
     */
    nil Pan(float delta_x, float delta_y);
    
    /**
     * @brief Moves camera along axes (Ctrl + mouse movement)
     * @param delta_x Horizontal movement delta
     * @param delta_y Vertical movement delta
     */
    nil MoveAlongAxes(float delta_x, float delta_y);
    
    // Zoom controls
    /**
     * @brief Zooms the camera
     * @param delta Zoom delta (positive = zoom in, negative = zoom out)
     */
    nil Zoom(float delta);
    
    /**
     * @brief Sets zoom factor
     * @param factor Zoom factor (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out)
     */
    nil SetZoomFactor(float factor);
    
    // Input handling
    /**
     * @brief Handles mouse button press
     * @param button Mouse button
     * @param action Button action (press/release)
     * @param x Mouse x coordinate
     * @param y Mouse y coordinate
     */
    nil HandleMouseButton(int button, int action, float x, float y);
    
    /**
     * @brief Handles mouse movement
     * @param x Mouse x coordinate
     * @param y Mouse y coordinate
     */
    nil HandleMouseMove(float x, float y);
    
    /**
     * @brief Handles mouse scroll
     * @param xoffset Scroll x offset
     * @param yoffset Scroll y offset
     */
    nil HandleMouseScroll(float xoffset, float yoffset);
    
    /**
     * @brief Handles keyboard input
     * @param key Key code
     * @param action Key action (press/release)
     */
    nil HandleKey(int key, int action);
    
    // Matrix generation
    /**
     * @brief Gets view matrix
     * @return Math::Matrix4 View matrix
     */
    Math::Matrix4 GetViewMatrix() const;
    
    /**
     * @brief Gets projection matrix
     * @return Math::Matrix4 Projection matrix
     */
    Math::Matrix4 GetProjectionMatrix() const;
    
    /**
     * @brief Updates camera (call each frame)
     * @param width Viewport width
     * @param height Viewport height
     */
    nil Update(int width, int height);
    
    // Utility methods
    /**
     * @brief Resets camera to default position
     */
    nil Reset();
    
    /**
     * @brief Fits view to show all objects
     * @param min_bounds Minimum bounding box
     * @param max_bounds Maximum bounding box
     */
    nil FitToBounds(const Math::Vector3& min_bounds, const Math::Vector3& max_bounds);
    
    /**
     * @brief Gets camera forward direction
     * @return Math::Vector3 Forward direction vector
     */
    Math::Vector3 GetForward() const;
    
    /**
     * @brief Gets camera right direction
     * @return Math::Vector3 Right direction vector
     */
    Math::Vector3 GetRight() const;
    
    /**
     * @brief Gets camera up direction
     * @return Math::Vector3 Up direction vector
     */
    Math::Vector3 GetUp() const;
};

} // namespace MentalEngine

#endif // MENTAL_CAMERA_H
