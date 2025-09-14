/**
 * @file Camera.cpp
 * @brief Implementation of the Camera class
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 */

#include "Camera.h"

namespace MentalEngine {

Camera::Camera() 
    : position(0.0f, 0.0f, 5.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , projection(CameraProjection::Perspective)
    , fov(45.0f)
    , aspect_ratio(16.0f / 9.0f)
    , near_plane(0.1f)
    , far_plane(1000.0f)
    , ortho_size(10.0f)
    , is_rotating(false)
    , is_panning(false)
    , is_zooming(false)
    , is_ctrl_pressed(false)
    , last_mouse_pos(0.0f, 0.0f)
    , mouse_delta(0.0f, 0.0f)
    , orbit_distance(5.0f)
    , orbit_angle_x(0.0f)
    , orbit_angle_y(0.0f)
    , min_orbit_distance(0.1f)
    , max_orbit_distance(1000.0f)
    , pan_speed(1.0f, 1.0f, 1.0f)
    , zoom_speed(0.1f)
    , zoom_factor(1.0f)
{
    __update_orbit_position();
}

nil Camera::SetProjection(CameraProjection proj) {
    projection = proj;
}

nil Camera::SetFieldOfView(float fov_degrees) {
    fov = fov_degrees;
}

nil Camera::SetOrthographicSize(float size) {
    ortho_size = size;
}

nil Camera::SetClippingPlanes(float near, float far) {
    near_plane = near;
    far_plane = far;
}

nil Camera::SetPosition(const Math::Vector3& pos) {
    position = pos;
    // Update orbit parameters based on new position
    Math::Vector3 direction = (position - target).normalized();
    orbit_distance = (position - target).length();
    orbit_angle_y = std::asin(direction.y);
    orbit_angle_x = std::atan2(direction.z, direction.x);
}

nil Camera::SetTarget(const Math::Vector3& new_target) {
    target = new_target;
    __update_orbit_position();
}

nil Camera::SetOrbitDistance(float distance) {
    orbit_distance = std::max(min_orbit_distance, std::min(max_orbit_distance, distance));
    __update_orbit_position();
}

nil Camera::SetOrbitAngles(float angle_x, float angle_y) {
    orbit_angle_x = angle_x;
    orbit_angle_y = angle_y;
    __constrain_orbit_angles();
    __update_orbit_position();
}

nil Camera::Orbit(float delta_x, float delta_y) {
    orbit_angle_x += delta_x * 0.01f;
    orbit_angle_y += delta_y * 0.01f;
    __constrain_orbit_angles();
    __update_orbit_position();
}

nil Camera::Pan(float delta_x, float delta_y) {
    Math::Vector3 right = GetRight();
    Math::Vector3 up = GetUp();
    
    Math::Vector3 pan_delta = (right * delta_x + up * delta_y) * pan_speed.x * 0.01f;
    position = position + pan_delta;
    target = target + pan_delta;
}

nil Camera::MoveAlongAxes(float delta_x, float delta_y) {
    Math::Vector3 right = GetRight();
    Math::Vector3 up = GetUp();
    Math::Vector3 forward = GetForward();
    
    // Движение по осям:
    // delta_x - движение влево/вправо (по оси X)
    // delta_y - движение вверх/вниз (по оси Y)
    // Дополнительно: движение вперед/назад по оси Z при движении мыши вверх/вниз
    
    Math::Vector3 move_delta = (right * delta_x + up * delta_y) * pan_speed.x * 0.01f;
    
    // Добавляем небольшое движение вперед/назад для более интуитивного управления
    Math::Vector3 forward_delta = forward * delta_y * pan_speed.z * 0.005f;
    
    position = position + move_delta + forward_delta;
    target = target + move_delta + forward_delta;
}

nil Camera::Zoom(float delta) {
    if (projection == CameraProjection::Perspective) {
        orbit_distance *= (1.0f - delta * zoom_speed);
        orbit_distance = std::max(min_orbit_distance, std::min(max_orbit_distance, orbit_distance));
        __update_orbit_position();
    } else {
        // Orthographic zoom
        ortho_size *= (1.0f - delta * zoom_speed);
        ortho_size = std::max(0.1f, ortho_size);
    }
}

nil Camera::SetZoomFactor(float factor) {
    zoom_factor = factor;
    if (projection == CameraProjection::Orthographic) {
        ortho_size = 10.0f / factor;
    }
}

nil Camera::HandleMouseButton(int button, int action, float x, float y) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        is_rotating = (action == GLFW_PRESS);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        is_panning = (action == GLFW_PRESS);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        is_zooming = (action == GLFW_PRESS);
    }
    
    if (action == GLFW_PRESS) {
        last_mouse_pos = Math::Vector2(x, y);
    }
}

nil Camera::HandleMouseMove(float x, float y) {
    mouse_delta = Math::Vector2(x - last_mouse_pos.x, last_mouse_pos.y - y);
    last_mouse_pos = Math::Vector2(x, y);
    
    if (is_ctrl_pressed && is_rotating) {
        // Ctrl + левая кнопка мыши = перемещение по осям
        MoveAlongAxes(mouse_delta.x, mouse_delta.y);
    } else if (is_rotating) {
        // Обычное вращение камеры
        Orbit(mouse_delta.x, mouse_delta.y);
    } else if (is_panning) {
        // Панорамирование
        Pan(mouse_delta.x, mouse_delta.y);
    }
}

nil Camera::HandleMouseScroll(float xoffset, float yoffset) {
    (void)xoffset; // Suppress unused parameter warning
    Zoom(yoffset);
}

nil Camera::HandleKey(int key, int action) {
    // Отслеживаем состояние клавиши Ctrl
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
        is_ctrl_pressed = (action == GLFW_PRESS);
    }
    
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_R:
                Reset();
                break;
            case GLFW_KEY_P:
                SetProjection(projection == CameraProjection::Perspective ? 
                             CameraProjection::Orthographic : CameraProjection::Perspective);
                break;
        }
    }
}

Math::Matrix4 Camera::GetViewMatrix() const {
    return Math::lookAt(position, target, up);
}

Math::Matrix4 Camera::GetProjectionMatrix() const {
    if (projection == CameraProjection::Perspective) {
        return Math::perspective(Math::radians(fov), aspect_ratio, near_plane, far_plane);
    } else {
        float half_size = ortho_size * 0.5f;
        return Math::orthographic(-half_size * aspect_ratio, half_size * aspect_ratio,
                                 -half_size, half_size, near_plane, far_plane);
    }
}

nil Camera::Update(int width, int height) {
    __update_aspect_ratio(width, height);
}

nil Camera::Reset() {
    position = Math::Vector3(0.0f, 0.0f, 5.0f);
    target = Math::Vector3(0.0f, 0.0f, 0.0f);
    up = Math::Vector3(0.0f, 1.0f, 0.0f);
    orbit_distance = 5.0f;
    orbit_angle_x = 0.0f;
    orbit_angle_y = 0.0f;
    ortho_size = 10.0f;
    zoom_factor = 1.0f;
    is_ctrl_pressed = false;
    __update_orbit_position();
}

nil Camera::FitToBounds(const Math::Vector3& min_bounds, const Math::Vector3& max_bounds) {
    Math::Vector3 center = (min_bounds + max_bounds) * 0.5f;
    Math::Vector3 size = max_bounds - min_bounds;
    float max_size = std::max({size.x, size.y, size.z});
    
    target = center;
    orbit_distance = max_size * 2.0f;
    ortho_size = max_size;
    __update_orbit_position();
}

Math::Vector3 Camera::GetForward() const {
    return (target - position).normalized();
}

Math::Vector3 Camera::GetRight() const {
    return GetForward().cross(up).normalized();
}

Math::Vector3 Camera::GetUp() const {
    return up;
}

nil Camera::__update_orbit_position() {
    float cos_y = std::cos(orbit_angle_y);
    position.x = target.x + orbit_distance * cos_y * std::cos(orbit_angle_x);
    position.y = target.y + orbit_distance * std::sin(orbit_angle_y);
    position.z = target.z + orbit_distance * cos_y * std::sin(orbit_angle_x);
}

nil Camera::__constrain_orbit_angles() {
    orbit_angle_y = std::max(-Math::radians(89.0f), std::min(Math::radians(89.0f), orbit_angle_y));
}

nil Camera::__update_aspect_ratio(int width, int height) {
    if (height > 0) {
        aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    }
}

} // namespace MentalEngine
