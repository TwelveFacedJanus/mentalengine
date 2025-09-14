#ifndef MENTAL_RENDERER_H
#define MENTAL_RENDERER_H

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../Core/Types.h"
#include <functional>

class Renderer {
private:
    // OpenGL Viewport variables
    GLuint viewport_framebuffer = 0;
    GLuint viewport_texture = 0;
    GLuint viewport_renderbuffer = 0;
    int viewport_width = 800;
    int viewport_height = 600;
    bool viewport_initialized = false;
    
    // Shader variables
    GLuint shader_program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    
    // Grid settings
    float grid_cell_size = 50.0f;  // Размер ячейки в пикселях
    float grid_line_width = 3.0f;  // Толщина линий сетки
    float grid_color[3] = {1.0f, 1.0f, 1.0f};  // Цвет линий сетки (RGB) - яркий белый
    bool show_grid = true;  // Показывать ли сетку

    nil __init_viewport();
    nil __cleanup_viewport();
    nil __init_shaders();
    nil __cleanup_shaders();
    nil __render_viewport_content();
    nil __render_grid();

public:
    Renderer() {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {return;}
        // Не инициализируем шейдеры здесь - они будут инициализированы при первом вызове RenderViewport
        shader_program = 0;  // Убеждаемся, что шейдеры не инициализированы
    }
    
    ~Renderer() {
        __cleanup_viewport();
        __cleanup_shaders();
    }
    
    nil DrawFrame(std::function<nil()> operation) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        operation();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    }
    
    // Viewport methods
    nil InitViewport(int width, int height);
    nil RenderViewport(int width, int height);
    GLuint GetViewportTexture() const { return viewport_texture; }
    int GetViewportWidth() const { return viewport_width; }
    int GetViewportHeight() const { return viewport_height; }
    
    // Grid methods
    nil SetGridVisible(bool visible) { show_grid = visible; }
    nil SetGridCellSize(float size) { grid_cell_size = size; }
    nil SetGridColor(float r, float g, float b) { 
        grid_color[0] = r; grid_color[1] = g; grid_color[2] = b; 
    }
    bool IsGridVisible() const { return show_grid; }
    float GetGridCellSize() const { return grid_cell_size; }
};

#endif // MENTAL_RENDERER_H
