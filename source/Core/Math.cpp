/**
 * @file Math.cpp
 * @brief Implementation of mathematical utilities
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 */

#include "Math.h"

namespace MentalEngine {
namespace Math {

Matrix4 translate(const Vector3& translation) {
    Matrix4 result;
    result.m[0][3] = translation.x;
    result.m[1][3] = translation.y;
    result.m[2][3] = translation.z;
    return result;
}

Matrix4 rotateX(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    
    return result;
}

Matrix4 rotateY(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    
    return result;
}

Matrix4 rotateZ(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;
    
    return result;
}

Matrix4 scale(const Vector3& scale) {
    Matrix4 result;
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    return result;
}

Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    Vector3 f = (target - eye).normalized();
    Vector3 s = f.cross(up).normalized();
    Vector3 u = s.cross(f);
    
    Matrix4 result;
    result.m[0][0] = s.x;
    result.m[1][0] = s.y;
    result.m[2][0] = s.z;
    result.m[0][1] = u.x;
    result.m[1][1] = u.y;
    result.m[2][1] = u.z;
    result.m[0][2] = -f.x;
    result.m[1][2] = -f.y;
    result.m[2][2] = -f.z;
    result.m[3][0] = -s.dot(eye);
    result.m[3][1] = -u.dot(eye);
    result.m[3][2] = f.dot(eye);
    
    return result;
}

Matrix4 perspective(float fov, float aspect, float near, float far) {
    Matrix4 result;
    float tanHalfFov = std::tan(fov / 2.0f);
    
    result.m[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(far + near) / (far - near);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * far * near) / (far - near);
    result.m[3][3] = 0.0f;
    
    return result;
}

Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 result;
    
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (far - near);
    result.m[0][3] = -(right + left) / (right - left);
    result.m[1][3] = -(top + bottom) / (top - bottom);
    result.m[2][3] = -(far + near) / (far - near);
    
    return result;
}

float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

float degrees(float radians) {
    return radians * 180.0f / M_PI;
}

} // namespace Math
} // namespace MentalEngine
