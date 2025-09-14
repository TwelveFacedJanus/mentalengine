/**
 * @file Renderer.cpp
 * @brief Implementation of the Renderer class
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file contains the implementation of the Renderer class methods,
 * including viewport management, shader compilation, and rendering operations.
 */

#include "Renderer.h"
#include <iostream>
#include <algorithm>
#include <vector>

/**
 * @brief Initializes the viewport with specified dimensions
 * 
 * Creates and configures the OpenGL framebuffer for viewport rendering
 * with the given width and height. If a viewport was previously initialized,
 * it will be cleaned up before creating the new one.
 * 
 * @param width Viewport width in pixels
 * @param height Viewport height in pixels
 */
nil Renderer::InitViewport(int width, int height) {
    std::cout << "InitViewport called with size: " << width << "x" << height << std::endl;
    
    viewport_width = width;
    viewport_height = height;
    
    // Очищаем предыдущий viewport если он был
    if (viewport_initialized) {
        __cleanup_viewport();
    }
    
    // Инициализируем шейдеры перед viewport
    if (shader_program == 0) {
        __init_shaders();
    }
    
    __init_viewport();
    viewport_initialized = true;
}

/**
 * @brief Renders the viewport content
 * 
 * Renders the current viewport content to the framebuffer. If the dimensions
 * have changed or the viewport is not initialized, it will be reinitialized.
 * Shaders are initialized on first call if not already done.
 * 
 * @param width Desired viewport width in pixels
 * @param height Desired viewport height in pixels
 */
nil Renderer::RenderViewport(int width, int height) {
    // Обновляем размер если изменился
    if (width != viewport_width || height != viewport_height) {
        InitViewport(width, height);
    }
    
    if (!viewport_initialized) {
        InitViewport(width, height);
    }
    
    // Инициализируем шейдеры только один раз
    if (shader_program == 0) {
        std::cout << "Initializing shaders for the first time..." << std::endl;
        __init_shaders();
    }
    
    // Рендерим содержимое в framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, viewport_framebuffer);
    glViewport(0, 0, viewport_width, viewport_height);
    __render_viewport_content();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Initializes the OpenGL viewport framebuffer
 * @private
 * 
 * Creates and configures the OpenGL framebuffer, texture, and renderbuffer
 * objects needed for viewport rendering. Sets up proper texture parameters
 * and framebuffer attachments.
 */
nil Renderer::__init_viewport() {
    // Создаем framebuffer
    glGenFramebuffers(1, &viewport_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewport_framebuffer);
    
    // Создаем texture для цвета
    glGenTextures(1, &viewport_texture);
    glBindTexture(GL_TEXTURE_2D, viewport_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewport_width, viewport_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewport_texture, 0);
    
    // Создаем renderbuffer для глубины
    glGenRenderbuffers(1, &viewport_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewport_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport_width, viewport_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, viewport_renderbuffer);
    
    // Проверяем статус framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Ошибка: Framebuffer не завершен!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Cleans up viewport OpenGL resources
 * @private
 * 
 * Deletes all OpenGL objects associated with the viewport including
 * framebuffer, texture, and renderbuffer objects.
 */
nil Renderer::__cleanup_viewport() {
    if (viewport_framebuffer) {
        glDeleteFramebuffers(1, &viewport_framebuffer);
        viewport_framebuffer = 0;
    }
    if (viewport_texture) {
        glDeleteTextures(1, &viewport_texture);
        viewport_texture = 0;
    }
    if (viewport_renderbuffer) {
        glDeleteRenderbuffers(1, &viewport_renderbuffer);
        viewport_renderbuffer = 0;
    }
    viewport_initialized = false;
}

/**
 * @brief Initializes and compiles OpenGL shaders
 * @private
 * 
 * Creates, compiles, and links vertex and fragment shaders for basic
 * rendering operations. Includes error checking and logging for shader
 * compilation and linking.
 */
nil Renderer::__init_shaders() {
    std::cout << "Initializing shaders..." << std::endl;
    
    // Простой vertex shader
    const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 vertexColor;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            vertexColor = aColor;
        }
    )";
    
    // Простой fragment shader
    const char* fragment_shader_source = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
    
    // Создаем vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    
    // Проверяем ошибки компиляции vertex shader
    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Создаем fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    
    // Проверяем ошибки компиляции fragment shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Создаем shader program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    
    // Проверяем ошибки линковки
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // Удаляем shader'ы, так как они уже связаны с program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    if (shader_program == 0) {
        std::cout << "ERROR: Failed to create shader program!" << std::endl;
    } else {
        std::cout << "Shaders initialized successfully, program ID: " << shader_program << std::endl;
    }
}

/**
 * @brief Cleans up shader OpenGL resources
 * @private
 * 
 * Deletes the OpenGL shader program and associated resources.
 */
nil Renderer::__cleanup_shaders() {
    if (shader_program) {
        glDeleteProgram(shader_program);
        shader_program = 0;
    }
}

/**
 * @brief Renders the main viewport content
 * @private
 * 
 * Renders the main viewport content including a gradient background,
 * a colored triangle, and the grid overlay. Uses modern OpenGL with
 * VAOs and VBOs for efficient rendering.
 */
nil Renderer::__render_viewport_content() {
    // Очищаем экран
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Используем наш shader program
    glUseProgram(shader_program);
    
    // Простой рендеринг - градиентный фон (используем современный OpenGL)
    // Вершины для фона (квадрат на весь экран)
    float background_vertices[] = {
        -1.0f, -1.0f, 0.0f,  // левый нижний
         1.0f, -1.0f, 0.0f,  // правый нижний
         1.0f,  1.0f, 0.0f,  // правый верхний
        -1.0f,  1.0f, 0.0f   // левый верхний
    };
    
    // Цвета для каждой вершины (градиент)
    float background_colors[] = {
        0.1f, 0.1f, 0.2f,  // темно-синий
        0.2f, 0.1f, 0.3f,  // фиолетовый
        0.1f, 0.2f, 0.4f,  // синий
        0.0f, 0.1f, 0.3f   // темно-синий
    };
    
    // Индексы для двух треугольников
    unsigned int background_indices[] = {
        0, 1, 2,  // первый треугольник
        2, 3, 0   // второй треугольник
    };
    
    // Создаем VAO (обязательно в современном OpenGL)
    GLuint VAO, VBO, colorVBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    glGenBuffers(1, &EBO);
    
    // Привязываем VAO
    glBindVertexArray(VAO);
    
    // Загружаем данные вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(background_vertices), background_vertices, GL_STATIC_DRAW);
    
    // Загружаем данные цветов
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(background_colors), background_colors, GL_STATIC_DRAW);
    
    // Загружаем индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(background_indices), background_indices, GL_STATIC_DRAW);
    
    // Настраиваем атрибуты вершин
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Рендерим фон
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Отключаем атрибуты
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    // Отвязываем VAO
    glBindVertexArray(0);
    
    // Очищаем буферы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &EBO);
    
    // Рендерим сетку
    __render_grid();
    
    // Рисуем простой треугольник в центре
    float triangle_vertices[] = {
         0.0f,  0.5f, 0.0f,  // верх
        -0.5f, -0.5f, 0.0f,  // левый нижний
         0.5f, -0.5f, 0.0f   // правый нижний
    };
    
    float triangle_colors[] = {
        1.0f, 0.5f, 0.0f,  // оранжевый
        0.0f, 1.0f, 0.5f,  // зеленый
        0.5f, 0.0f, 1.0f   // фиолетовый
    };
    
    // Создаем VAO для треугольника
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    
    // Привязываем VAO
    glBindVertexArray(VAO);
    
    // Загружаем данные треугольника
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);
    
    // Настраиваем атрибуты
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Рендерим треугольник
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // Отключаем атрибуты
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    // Отвязываем VAO
    glBindVertexArray(0);
    
    // Очищаем буферы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorVBO);
}

/**
 * @brief Renders the grid overlay
 * @private
 * 
 * Renders a configurable grid overlay on top of the viewport content.
 * Grid properties such as visibility, cell size, and color can be
 * controlled through the public interface methods.
 */
nil Renderer::__render_grid() {
    if (!show_grid) return;
    
    // Используем наш shader program
    glUseProgram(shader_program);
    
    // Устанавливаем толщину линий сетки
    glLineWidth(grid_line_width);
    
    // Простая сетка с фиксированным размером
    float cell_size = 0.15f; // 15% от размера viewport
    
    // Подсчитываем количество линий
    int num_vertical = (int)(2.0f / cell_size) + 1;
    int num_horizontal = (int)(2.0f / cell_size) + 1;
    
    // Создаем массивы для вершин и цветов
    std::vector<float> vertices;
    std::vector<float> colors;
    
    // Генерируем вертикальные линии
    for (int i = 0; i < num_vertical; i++) {
        float x = -1.0f + i * cell_size;
        if (x > 1.0f) x = 1.0f;
        
        // Две вершины для линии
        vertices.push_back(x); vertices.push_back(-1.0f); vertices.push_back(0.0f);
        vertices.push_back(x); vertices.push_back(1.0f); vertices.push_back(0.0f);
        
        // Цвет для обеих вершин
        colors.push_back(grid_color[0]); colors.push_back(grid_color[1]); colors.push_back(grid_color[2]);
        colors.push_back(grid_color[0]); colors.push_back(grid_color[1]); colors.push_back(grid_color[2]);
    }
    
    // Генерируем горизонтальные линии
    for (int i = 0; i < num_horizontal; i++) {
        float y = -1.0f + i * cell_size;
        if (y > 1.0f) y = 1.0f;
        
        // Две вершины для линии
        vertices.push_back(-1.0f); vertices.push_back(y); vertices.push_back(0.0f);
        vertices.push_back(1.0f); vertices.push_back(y); vertices.push_back(0.0f);
        
        // Цвет для обеих вершин
        colors.push_back(grid_color[0]); colors.push_back(grid_color[1]); colors.push_back(grid_color[2]);
        colors.push_back(grid_color[0]); colors.push_back(grid_color[1]); colors.push_back(grid_color[2]);
    }
    
    // Создаем VAO для сетки
    GLuint VAO, VBO, colorVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    
    // Привязываем VAO
    glBindVertexArray(VAO);
    
    // Загружаем данные вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Загружаем данные цветов
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    
    // Настраиваем атрибуты
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Рендерим линии
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);
    
    // Отключаем атрибуты
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    // Отвязываем VAO
    glBindVertexArray(0);
    
    // Очищаем буферы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &colorVBO);
    
    // Сбрасываем толщину линии
    glLineWidth(1.0f);
}
