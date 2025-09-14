/**
 * @file Math.h
 * @brief Mathematical utilities for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file contains mathematical utilities including vector and matrix operations
 * needed for 3D graphics, camera systems, and transformations.
 */

#ifndef MENTAL_MATH_H
#define MENTAL_MATH_H

#include <cmath>
#include <array>

namespace MentalEngine {
namespace Math {

/**
 * @struct Vector2
 * @brief 2D vector representation
 */
struct Vector2 {
    float x, y;
    
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }
    
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2 normalized() const {
        float len = length();
        if (len == 0.0f) return Vector2(0, 0);
        return *this / len;
    }
    
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
};

/**
 * @struct Vector3
 * @brief 3D vector representation
 */
struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vector3 normalized() const {
        float len = length();
        if (len == 0.0f) return Vector3(0, 0, 0);
        return *this / len;
    }
    
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

/**
 * @struct Vector4
 * @brief 4D vector representation
 */
struct Vector4 {
    float x, y, z, w;
    
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
};

/**
 * @struct Matrix4
 * @brief 4x4 matrix representation
 */
struct Matrix4 {
    std::array<std::array<float, 4>, 4> m;
    
    Matrix4() {
        // Initialize as identity matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    Vector4 operator*(const Vector4& v) const {
        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }
    
    // Get pointer to data for OpenGL
    const float* data() const {
        return &m[0][0];
    }
};

/**
 * @brief Create translation matrix
 * @param translation Translation vector
 * @return Matrix4 Translation matrix
 */
Matrix4 translate(const Vector3& translation);

/**
 * @brief Create rotation matrix around X axis
 * @param angle Angle in radians
 * @return Matrix4 Rotation matrix
 */
Matrix4 rotateX(float angle);

/**
 * @brief Create rotation matrix around Y axis
 * @param angle Angle in radians
 * @return Matrix4 Rotation matrix
 */
Matrix4 rotateY(float angle);

/**
 * @brief Create rotation matrix around Z axis
 * @param angle Angle in radians
 * @return Matrix4 Rotation matrix
 */
Matrix4 rotateZ(float angle);

/**
 * @brief Create scale matrix
 * @param scale Scale vector
 * @return Matrix4 Scale matrix
 */
Matrix4 scale(const Vector3& scale);

/**
 * @brief Create look-at matrix
 * @param eye Camera position
 * @param target Target position
 * @param up Up vector
 * @return Matrix4 Look-at matrix
 */
Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

/**
 * @brief Create perspective projection matrix
 * @param fov Field of view in radians
 * @param aspect Aspect ratio
 * @param near Near plane distance
 * @param far Far plane distance
 * @return Matrix4 Perspective projection matrix
 */
Matrix4 perspective(float fov, float aspect, float near, float far);

/**
 * @brief Create orthographic projection matrix
 * @param left Left plane
 * @param right Right plane
 * @param bottom Bottom plane
 * @param top Top plane
 * @param near Near plane
 * @param far Far plane
 * @return Matrix4 Orthographic projection matrix
 */
Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);

/**
 * @brief Convert degrees to radians
 * @param degrees Angle in degrees
 * @return float Angle in radians
 */
float radians(float degrees);

/**
 * @brief Convert radians to degrees
 * @param radians Angle in radians
 * @return float Angle in degrees
 */
float degrees(float radians);

} // namespace Math
} // namespace MentalEngine

#endif // MENTAL_MATH_H
