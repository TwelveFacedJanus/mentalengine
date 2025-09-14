/**
 * @file Renderer.h
 * @brief OpenGL-based rendering system for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file defines the Renderer class, which provides a high-level interface
 * for OpenGL rendering operations. It includes viewport management, shader
 * handling, and grid rendering capabilities.
 */

#ifndef MENTAL_RENDERER_H
#define MENTAL_RENDERER_H

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../Core/Types.h"
#include "../Camera/Camera.h"
#include <functional>

/**
 * @class Renderer
 * @brief Main rendering class that handles OpenGL operations
 * 
 * The Renderer class provides a comprehensive interface for OpenGL rendering
 * operations including viewport management, shader compilation and management,
 * and grid rendering. It abstracts away the complexity of OpenGL setup and
 * provides a clean API for rendering operations.
 * 
 * Key features:
 * - Viewport management with framebuffer support
 * - Automatic shader compilation and management
 * - Configurable grid rendering
 * - Modern OpenGL 3.3+ support
 * 
 * @note This class requires a valid OpenGL context to function properly
 */
class Renderer {
private:
    // OpenGL Viewport variables
    GLuint viewport_framebuffer = 0;    ///< OpenGL framebuffer object for viewport rendering
    GLuint viewport_texture = 0;        ///< OpenGL texture object for viewport color buffer
    GLuint viewport_renderbuffer = 0;   ///< OpenGL renderbuffer object for depth/stencil
    int viewport_width = 800;           ///< Current viewport width in pixels
    int viewport_height = 600;          ///< Current viewport height in pixels
    bool viewport_initialized = false;  ///< Flag indicating if viewport is initialized
    
    // Shader variables
    GLuint shader_program = 0;    ///< OpenGL shader program ID
    GLuint vertex_shader = 0;     ///< OpenGL vertex shader ID
    GLuint fragment_shader = 0;   ///< OpenGL fragment shader ID
    GLint view_matrix_location = -1;      ///< View matrix uniform location
    GLint projection_matrix_location = -1; ///< Projection matrix uniform location
    GLint model_matrix_location = -1;     ///< Model matrix uniform location
    
    // Grid settings
    float grid_cell_size = 50.0f;  ///< Grid cell size in pixels
    float grid_line_width = 3.0f;  ///< Grid line thickness
    float grid_color[3] = {1.0f, 1.0f, 1.0f};  ///< Grid line color (RGB) - bright white
    bool show_grid = true;  ///< Flag to show/hide grid
    
    // Camera system
    std::shared_ptr<MentalEngine::Camera> camera;  ///< Camera instance

    /**
     * @brief Initializes the OpenGL viewport framebuffer
     * @private
     */
    nil __init_viewport();
    
    /**
     * @brief Cleans up viewport OpenGL resources
     * @private
     */
    nil __cleanup_viewport();
    
    /**
     * @brief Initializes and compiles OpenGL shaders
     * @private
     */
    nil __init_shaders();
    
    /**
     * @brief Cleans up shader OpenGL resources
     * @private
     */
    nil __cleanup_shaders();
    
    /**
     * @brief Renders the main viewport content
     * @private
     */
    nil __render_viewport_content();
    
    /**
     * @brief Renders the grid overlay
     * @private
     */
    nil __render_grid();

public:
    /**
     * @brief Constructor - initializes the renderer
     * 
     * Initializes GLEW (OpenGL Extension Wrangler) and sets up the renderer
     * for use. Shaders are not initialized here but will be created on first
     * viewport render call.
     */
    Renderer() {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {return;}
        // Shaders are not initialized here - they will be initialized on first RenderViewport call
        shader_program = 0;  // Ensure shaders are not initialized
        
        // Initialize camera
        camera = std::make_shared<MentalEngine::Camera>();
    }
    
    /**
     * @brief Destructor - cleans up OpenGL resources
     * 
     * Automatically cleans up all allocated OpenGL resources including
     * viewport framebuffers and shader programs.
     */
    ~Renderer() {
        __cleanup_viewport();
        __cleanup_shaders();
    }
    
    /**
     * @brief Draws a single frame with the specified operation
     * 
     * Clears the screen, executes the provided operation, and sets the
     * clear color for the next frame.
     * 
     * @param operation Function to execute during frame rendering
     */
    nil DrawFrame(std::function<nil()> operation) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        operation();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    }
    
    // Viewport methods
    /**
     * @brief Initializes the viewport with specified dimensions
     * 
     * Creates and configures the OpenGL framebuffer for viewport rendering
     * with the given width and height.
     * 
     * @param width Viewport width in pixels
     * @param height Viewport height in pixels
     */
    nil InitViewport(int width, int height);
    
    /**
     * @brief Renders the viewport content
     * 
     * Renders the current viewport content to the framebuffer. If the
     * dimensions have changed, the viewport will be reinitialized.
     * 
     * @param width Desired viewport width in pixels
     * @param height Desired viewport height in pixels
     */
    nil RenderViewport(int width, int height);
    
    /**
     * @brief Gets the viewport texture ID
     * @return GLuint OpenGL texture ID for the viewport
     */
    GLuint GetViewportTexture() const { return viewport_texture; }
    
    /**
     * @brief Gets the current viewport width
     * @return int Current viewport width in pixels
     */
    int GetViewportWidth() const { return viewport_width; }
    
    /**
     * @brief Gets the current viewport height
     * @return int Current viewport height in pixels
     */
    int GetViewportHeight() const { return viewport_height; }
    
    // Grid methods
    /**
     * @brief Sets the grid visibility
     * @param visible True to show grid, false to hide
     */
    nil SetGridVisible(bool visible) { show_grid = visible; }
    
    /**
     * @brief Sets the grid cell size
     * @param size Cell size in pixels
     */
    nil SetGridCellSize(float size) { grid_cell_size = size; }
    
    /**
     * @brief Sets the grid color
     * @param r Red component (0.0 - 1.0)
     * @param g Green component (0.0 - 1.0)
     * @param b Blue component (0.0 - 1.0)
     */
    nil SetGridColor(float r, float g, float b) { 
        grid_color[0] = r; grid_color[1] = g; grid_color[2] = b; 
    }
    
    /**
     * @brief Gets the grid visibility state
     * @return bool True if grid is visible, false otherwise
     */
    bool IsGridVisible() const { return show_grid; }
    
    /**
     * @brief Gets the current grid cell size
     * @return float Current cell size in pixels
     */
    float GetGridCellSize() const { return grid_cell_size; }
    
    // Camera methods
    /**
     * @brief Gets the camera instance
     * @return std::shared_ptr<MentalEngine::Camera> Camera instance
     */
    std::shared_ptr<MentalEngine::Camera> GetCamera() const { return camera; }
    
    /**
     * @brief Sets the camera instance
     * @param cam Camera instance to set
     */
    nil SetCamera(std::shared_ptr<MentalEngine::Camera> cam) { camera = cam; }
    
    /**
     * @brief Renders lines from a list of points
     * @param points Vector of line points (pairs of start/end points)
     * @param color Line color (RGB)
     * @param line_width Line width in pixels
     */
    nil RenderLines(const std::vector<MentalEngine::Math::Vector2>& points, const MentalEngine::Math::Vector3& color = MentalEngine::Math::Vector3(1.0f, 1.0f, 1.0f), float line_width = 2.0f);
};

#endif // MENTAL_RENDERER_H
