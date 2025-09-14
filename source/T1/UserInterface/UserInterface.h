/**
 * @file UserInterface.h
 * @brief ImGui-based user interface system for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file defines the UserInterface template class and ConsoleRedirectBuffer
 * class, which provide a comprehensive ImGui-based user interface system
 * with docking, console output redirection, and viewport integration.
 */

#ifndef MENTAL_USER_INTERFACE_H
#define MENTAL_USER_INTERFACE_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>

#include "../../Core/Types.h"
#include "../Renderer/Renderer.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Forward declaration
template <typename T> class UserInterface;
class ConsoleRedirectBuffer;

/**
 * @enum ToolType
 * @brief Available drawing tools
 */
enum class ToolType {
    None,       ///< No tool selected
    Line,       ///< Line drawing tool
    Rectangle,  ///< Rectangle drawing tool
    Circle      ///< Circle drawing tool
};

// Forward declaration for method
template <typename T> void __add_console_output_impl(void* ui, const std::string& text);

/**
 * @class ConsoleRedirectBuffer
 * @brief Custom streambuf for redirecting console output to ImGui
 * 
 * This class extends std::streambuf to intercept console output (std::cout, std::cerr)
 * and redirect it to the ImGui console window. It provides thread-safe output
 * redirection with automatic line buffering.
 */
class ConsoleRedirectBuffer : public std::streambuf {
private:
    void* ui;  ///< Pointer to the UserInterface instance
    
public:
    /**
     * @brief Constructor
     * @param ui Pointer to the UserInterface instance
     */
    ConsoleRedirectBuffer(void* ui) : ui(ui) {}
    
protected:
    /**
     * @brief Handles single character overflow
     * @param c Character to output
     * @return int_type Character processed
     * @protected
     */
    virtual int_type overflow(int_type c) override {
        if (c != EOF) {
            char ch = static_cast<char>(c);
            __add_console_output_impl<GLFWwindow>(ui, std::string(1, ch));
        }
        return c;
    }
    
    /**
     * @brief Handles multiple character output
     * @param s Character buffer
     * @param count Number of characters to output
     * @return std::streamsize Number of characters processed
     * @protected
     */
    virtual std::streamsize xsputn(const char* s, std::streamsize count) override {
        __add_console_output_impl<GLFWwindow>(ui, std::string(s, count));
        return count;
    }
};

/**
 * @class UserInterface
 * @brief Template class for ImGui-based user interface system
 * 
 * The UserInterface class provides a comprehensive ImGui-based user interface
 * system with docking support, console output redirection, viewport integration,
 * and various UI panels. It manages the entire UI lifecycle and provides
 * a clean interface for rendering UI elements.
 * 
 * @tparam T Window type (defaults to GLFWwindow)
 * 
 * Key features:
 * - ImGui integration with docking support
 * - Console output redirection and management
 * - Viewport rendering integration
 * - Customizable UI panels (toolbox, console, properties, etc.)
 * - Font loading and management
 * - Thread-safe console output
 * 
 * @note This class requires a valid OpenGL context and ImGui initialization
 */
template <typename T = GLFWwindow>
class UserInterface
{
private:
    T* pWindow = nullptr;                    ///< Pointer to the window
    ImGuiContext *pCTX = nullptr;           ///< ImGui context
    ImGuiIO *pIO = nullptr;                 ///< ImGui IO interface
    class Renderer* pRenderer = nullptr;    ///< Pointer to the renderer

    bool show_demo_window = true;           ///< Flag to show/hide ImGui demo window
    bool mouse_over_viewport = false;       ///< Flag indicating if mouse is over viewport

    // Drawing tools
    ToolType current_tool = ToolType::None; ///< Currently selected tool
    bool is_drawing = false;                ///< Currently drawing
    MentalEngine::Math::Vector2 line_start;               ///< Line start point
    MentalEngine::Math::Vector2 line_end;                 ///< Line end point
    std::vector<MentalEngine::Math::Vector2> line_points; ///< All line points for rendering

    // Console system
    std::vector<std::string> console_output;        ///< Console output buffer
    std::string console_input;                      ///< Console input buffer
    bool console_scroll_to_bottom = true;           ///< Auto-scroll flag
    std::mutex console_mutex;                       ///< Console thread safety mutex
    static const size_t MAX_CONSOLE_LINES = 1000;  ///< Maximum console lines
    ConsoleRedirectBuffer* cout_buffer = nullptr;   ///< stdout redirect buffer
    ConsoleRedirectBuffer* cerr_buffer = nullptr;   ///< stderr redirect buffer

    /**
     * @brief Loads default fonts for the UI
     * @private
     */
    nil __load_default_fonts();
    
    /**
     * @brief Initializes console output redirection
     * @private
     */
    nil __init_console_redirect();
    
    /**
     * @brief Cleans up console output redirection
     * @private
     */
    nil __cleanup_console_redirect();

public:
    /**
     * @brief Adds text to console output
     * @param text Text to add to console
     */
    nil __add_console_output(const std::string& text);
    
    /**
     * @brief Constructor - initializes the user interface
     * @param pWindow Pointer to the window
     * @param pRenderer Pointer to the renderer
     */
    UserInterface(T* pWindow, class Renderer* pRenderer);
    
    /**
     * @brief Starts a new ImGui frame
     */
    nil NewFrame();
    
    /**
     * @brief Ends the current ImGui frame
     */
    nil EndFrame();
    
    /**
     * @brief Creates the main docking space
     */
    nil Dockspace();
    
    /**
     * @brief Renders the main menu bar
     */
    nil MainMenu();
    
    /**
     * @brief Renders the main UI frame
     */
    nil DrawFrame();
    
    /**
     * @brief Renders the viewport panel
     */
    nil Viewport();
    
    /**
     * @brief Renders the toolbox panel
     */
    nil Toolbox();
    
    /**
     * @brief Renders the console panel
     */
    nil Console(); 
    
    /**
     * @brief Checks if mouse is over the viewport area
     * @return true if mouse is over viewport, false otherwise
     */
    bool IsMouseOverViewport();
    
    /**
     * @brief Handles mouse input for drawing tools
     * @param button Mouse button
     * @param action Button action (press/release)
     * @param x Mouse x coordinate
     * @param y Mouse y coordinate
     */
    nil HandleDrawingInput(int button, int action, float x, float y);
    
    /**
     * @brief Handles mouse movement for drawing tools
     * @param x Mouse x coordinate
     * @param y Mouse y coordinate
     */
    nil HandleDrawingMouseMove(float x, float y);
    
    /**
     * @brief Destructor - cleans up resources
     */
    ~UserInterface();
};

/**
 * @brief Renders the toolbox panel
 * @tparam T Window type
 * 
 * Creates a toolbox panel with drawing tools and controls.
 * Includes tools for drawing lines, rectangles, and circles.
 */
template <typename T>
nil UserInterface<T>::Toolbox() {
    ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus);
    
    ImGui::Text("Drawing Tools");
    ImGui::Separator();
    
    // Tool selection buttons
    if (ImGui::Button("Select", ImVec2(80, 30))) {
        current_tool = ToolType::None;
        is_drawing = false;
    }
    if (current_tool == ToolType::None) {
        ImGui::SameLine();
        ImGui::Text("✓");
    }
    
    if (ImGui::Button("Line", ImVec2(80, 30))) {
        current_tool = ToolType::Line;
        is_drawing = false;
    }
    if (current_tool == ToolType::Line) {
        ImGui::SameLine();
        ImGui::Text("✓");
    }
    
    if (ImGui::Button("Rectangle", ImVec2(80, 30))) {
        current_tool = ToolType::Rectangle;
        is_drawing = false;
    }
    if (current_tool == ToolType::Rectangle) {
        ImGui::SameLine();
        ImGui::Text("✓");
    }
    
    if (ImGui::Button("Circle", ImVec2(80, 30))) {
        current_tool = ToolType::Circle;
        is_drawing = false;
    }
    if (current_tool == ToolType::Circle) {
        ImGui::SameLine();
        ImGui::Text("✓");
    }
    
    ImGui::Separator();
    
    // Tool info
    ImGui::Text("Current Tool:");
    switch (current_tool) {
        case ToolType::None:
            ImGui::Text("Select Tool");
            break;
        case ToolType::Line:
            ImGui::Text("Line Tool");
            ImGui::Text("Click and drag to draw");
            break;
        case ToolType::Rectangle:
            ImGui::Text("Rectangle Tool");
            ImGui::Text("Click and drag to draw");
            break;
        case ToolType::Circle:
            ImGui::Text("Circle Tool");
            ImGui::Text("Click and drag to draw");
            break;
    }
    
    ImGui::Separator();
    
    // Clear button
    if (ImGui::Button("Clear All", ImVec2(80, 30))) {
        line_points.clear();
        is_drawing = false;
    }
    
    ImGui::End();
}

/**
 * @brief Renders the console panel
 * @tparam T Window type
 * 
 * Creates an interactive console panel with output display, input field,
 * and command processing. Supports console output redirection and
 * basic commands like clear, help, and quit.
 */
template <typename T>
nil UserInterface<T>::Console() {
    ImGui::Begin("Console");
    
    // Кнопки управления
    if (ImGui::Button("Clear")) {
        std::lock_guard<std::mutex> lock(console_mutex);
        console_output.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scroll to Bottom")) {
        console_scroll_to_bottom = true;
    }
    
    ImGui::Separator();
    
    // Область вывода терминала
    ImGui::BeginChild("ConsoleOutput", ImVec2(0, -ImGui::GetFrameHeight() - 10), true);
    
    // Отображаем вывод терминала
    {
        std::lock_guard<std::mutex> lock(console_mutex);
        for (const auto& line : console_output) {
            ImGui::TextUnformatted(line.c_str());
        }
    }
    
    // Автоматическая прокрутка вниз
    if (console_scroll_to_bottom) {
        ImGui::SetScrollHereY(1.0f);
        console_scroll_to_bottom = false;
    }
    
    ImGui::EndChild();
    
    // Поле ввода команды
    ImGui::PushItemWidth(-1);
    static char input_buf[256] = "";
    if (ImGui::InputText("##ConsoleInput", input_buf, sizeof(input_buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (strlen(input_buf) > 0) {
            std::string command = std::string(input_buf);
            // Добавляем команду в вывод
            __add_console_output("> " + command);
            
            // Здесь можно добавить логику выполнения команды
            if (command == "clear") {
                std::lock_guard<std::mutex> lock(console_mutex);
                console_output.clear();
            } else if (command == "help") {
                __add_console_output("Доступные команды:");
                __add_console_output("  clear - очистить консоль");
                __add_console_output("  help - показать эту справку");
                __add_console_output("  quit - выйти из приложения");
            } else if (command == "quit") {
                __add_console_output("Выход из приложения...");
                // Здесь можно добавить логику выхода
            } else {
                __add_console_output("Неизвестная команда: " + command);
            }
            
            input_buf[0] = '\0';
            console_scroll_to_bottom = true;
        }
    }
    ImGui::PopItemWidth();
    
    // Фокус на поле ввода
    if (ImGui::IsWindowAppearing()) {
        ImGui::SetKeyboardFocusHere(-1);
    }
    
    ImGui::End();
}

/**
 * @brief Renders the viewport panel
 * @tparam T Window type
 * 
 * Creates a viewport panel that displays the rendered content from the
 * renderer. Integrates with the Renderer class to show the current
 * viewport texture in the ImGui interface.
 */
template <typename T>
nil UserInterface<T>::Viewport() {
    ImGui::Begin("Viewport");
    
    // Устанавливаем флаг, если мышь находится над viewport
    mouse_over_viewport = ImGui::IsWindowHovered();
    
    if (pRenderer) {
        // Получаем размер доступной области
        ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
        int width = static_cast<int>(viewport_panel_size.x);
        int height = static_cast<int>(viewport_panel_size.y);
        
        // Рендерим viewport через Renderer
        pRenderer->RenderViewport(width, height);
        
        // Рендерим нарисованные линии
        if (!line_points.empty() && line_points.size() % 2 == 0) {
            pRenderer->RenderLines(line_points, MentalEngine::Math::Vector3(1.0f, 0.0f, 0.0f), 2.0f);
        }
        
        // Рендерим текущую линию, если рисуем
        if (is_drawing && current_tool == ToolType::Line) {
            std::vector<MentalEngine::Math::Vector2> current_line = {line_start, line_end};
            pRenderer->RenderLines(current_line, MentalEngine::Math::Vector3(0.0f, 1.0f, 0.0f), 2.0f);
        }
        
        // Отображаем texture в ImGui
        GLuint texture = pRenderer->GetViewportTexture();
        if (texture != 0) {
            ImGui::Image((void*)(intptr_t)texture, 
                         ImVec2(width, height), 
                         ImVec2(0, 1), ImVec2(1, 0));
        } else {
            ImGui::Text("Viewport texture не создан");
        }
    } else {
        ImGui::Text("Renderer не инициализирован");
    }
    
    ImGui::End();
}

/**
 * @brief Renders the main UI frame
 * @tparam T Window type
 * 
 * Main UI rendering method that orchestrates all UI panels and components.
 * Handles the complete UI frame including docking, menus, viewport, toolbox,
 * console, and various property panels.
 */
template <typename T>
nil UserInterface<T>::DrawFrame() {
    this->NewFrame();
    this->Dockspace();
    this->MainMenu();
    this->Viewport();
    this->Toolbox();
    this->Console();
    // Демонстрационное окно
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    // Окно свойств (всегда видимо)
    ImGui::Begin("Properties");
    ImGui::Text("MentalEngine v1.0");
    ImGui::Separator();
    ImGui::Text("FPS: %.1f", this->pIO->Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / this->pIO->Framerate);
    ImGui::Separator();
    if (ImGui::Button("Toggle Demo")) {
        this->show_demo_window = !this->show_demo_window;
    }
    ImGui::SameLine();
    ImGui::End();
    
    // Окно настроек сетки
    ImGui::Begin("Grid Settings");
    if (pRenderer) {
        // Переключатель видимости сетки
        bool grid_visible = pRenderer->IsGridVisible();
        if (ImGui::Checkbox("Show Grid", &grid_visible)) {
            pRenderer->SetGridVisible(grid_visible);
        }
        
        // Настройка размера ячеек
        float cell_size = pRenderer->GetGridCellSize();
        if (ImGui::SliderFloat("Cell Size", &cell_size, 10.0f, 200.0f)) {
            pRenderer->SetGridCellSize(cell_size);
        }
        
        // Настройка цвета сетки
        static float grid_color[3] = {0.8f, 0.8f, 0.8f};
        if (ImGui::ColorEdit3("Grid Color", grid_color)) {
            pRenderer->SetGridColor(grid_color[0], grid_color[1], grid_color[2]);
        }
        
        ImGui::Separator();
        ImGui::Text("Grid Info:");
        ImGui::Text("Cell Size: %.1f px", cell_size);
        ImGui::Text("Viewport: %dx%d", pRenderer->GetViewportWidth(), pRenderer->GetViewportHeight());
    } else {
        ImGui::Text("Renderer не инициализирован");
    }
    ImGui::End();

    

    // Окно иерархии
    ImGui::Begin("Hierarchy");
    if (ImGui::TreeNode("Scene")) {
        if (ImGui::TreeNode("GameObjects")) {
            ImGui::Text("Object 1");
            ImGui::Text("Object 2");
            ImGui::Text("Object 3");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Lights")) {
            ImGui::Text("Directional Light");
            ImGui::Text("Point Light 1");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::End();
    this->EndFrame();
}

/**
 * @brief Starts a new ImGui frame
 * @tparam T Window type
 * 
 * Initializes a new ImGui frame by calling the appropriate ImGui
 * new frame functions for OpenGL3 and GLFW backends.
 */
template <typename T>
nil UserInterface<T>::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * @brief Renders the main menu bar
 * @tparam T Window type
 * 
 * Creates the main menu bar with File and View menus. Provides
 * basic application controls and options.
 */
template <typename T>
nil UserInterface<T>::MainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                // Логика открытия файла
            }
            if (ImGui::MenuItem("Save")) {
                // Логика сохранения
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // Логика выхода
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Demo window", nullptr, &show_demo_window)) {
                // Переключение демо окна
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

/**
 * @brief Creates the main docking space
 * @tparam T Window type
 * 
 * Sets up the main docking space for the ImGui interface, allowing
 * users to dock and arrange windows as needed. Configures the
 * docking area to fill the entire viewport.
 */
template <typename T>
nil UserInterface<T>::Dockspace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Настройки окна для dockspace
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground;

    // Стиль для dockspace
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // Создаем окно для dockspace
    ImGui::Begin("DockSpace", nullptr, window_flags);

    // Создаем dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // Dockspace готов к использованию
    // Пользователи могут перетаскивать окна для создания собственного макета

    ImGui::End();

    // Восстанавливаем стиль
    ImGui::PopStyleVar(3);
}

/**
 * @brief Ends the current ImGui frame
 * @tparam T Window type
 * 
 * Finalizes the current ImGui frame by rendering all ImGui draw data
 * and handling multi-viewport updates if enabled.
 */
template <typename T>
nil UserInterface<T>::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Обновление viewports (для поддержки множественных окон)
    if (pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

/**
 * @brief Constructor - initializes the user interface
 * @tparam T Window type
 * @param pWindow Pointer to the window
 * @param pRenderer Pointer to the renderer
 * 
 * Initializes ImGui with GLFW and OpenGL3 backends, sets up docking
 * and viewport support, loads custom fonts, and initializes console
 * output redirection.
 */
template <typename T>
UserInterface<T>::UserInterface(T* pWindow, class Renderer* pRenderer) {
    IMGUI_CHECKVERSION();
    pCTX = ImGui::CreateContext();
    pIO = &ImGui::GetIO();
    (void)pIO;

    pIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    pIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Отключаем ImGui курсоры, чтобы использовать только GLFW
    //io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    // Дополнительно отключаем курсоры через ImGuiIO
    //io->WantCaptureMouse = false;

    // Настройка стиля ImGui
    ImGui::StyleColorsDark();

    // Настройка платформы и рендерера
    if (!ImGui_ImplGlfw_InitForOpenGL(pWindow, true)) {
        std::cerr << "Ошибка инициализации ImGui GLFW backend" << std::endl;
        return;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Ошибка инициализации ImGui OpenGL3 backend" << std::endl;
        return;
    }

    // Загружаем кастомные шрифты
    __load_default_fonts();

    std::cout << "ImGui успешно инициализирован" << std::endl;

    // Инициализируем перенаправление консоли
    __init_console_redirect();

    this->pWindow = pWindow;
    this->pRenderer = pRenderer;
    
    // Добавляем тестовую линию для проверки рендеринга
    line_points.push_back(MentalEngine::Math::Vector2(-0.5f, -0.5f));
    line_points.push_back(MentalEngine::Math::Vector2(0.5f, 0.5f));
    
    return;
}

/**
 * @brief Destructor - cleans up resources
 * @tparam T Window type
 * 
 * Cleans up console redirection, shuts down ImGui backends,
 * and destroys the ImGui context.
 */
template <typename T>
UserInterface<T>::~UserInterface() {
    // Очищаем перенаправление консоли
    __cleanup_console_redirect();
    
    if (this->pCTX) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(this->pCTX);
        this->pCTX = nullptr;
        this->pIO = nullptr;
    }
}

/**
 * @brief Loads default fonts for the UI
 * @tparam T Window type
 * @private
 * 
 * Loads custom fonts (SF Pro Text) for the ImGui interface.
 * Falls back to default font if custom fonts are not available.
 */
template <typename T>
nil UserInterface<T>::__load_default_fonts() {
    // Загружаем SF Pro Text как основной шрифт
    const char* fontPath = "resources/fonts/SFProText-Regular.ttf";

    // Проверяем существование файла
    FILE* file = fopen(fontPath, "r");
    if (!file) {
        std::cerr << "Предупреждение: Не удалось найти шрифт " << fontPath << ", используется шрифт по умолчанию" << std::endl;
        return;
    }
    fclose(file);

    // Очищаем существующие шрифты
    this->pIO->Fonts->Clear();

    // Загружаем SF Pro Text с разными размерами
    ImFont* sfProFont16 = this->pIO->Fonts->AddFontFromFileTTF(
        fontPath,
        16.0f,                    // Размер шрифта в пикселях
        nullptr,                  // Конфигурация шрифта (опционально)
        this->pIO->Fonts->GetGlyphRangesDefault() // Диапазоны символов
    );

    // Загружаем дополнительный размер для заголовков
    ImFont* sfProFont24 = this->pIO->Fonts->AddFontFromFileTTF(
        fontPath,
        24.0f,                    // Больший размер для заголовков
        nullptr,
        this->pIO->Fonts->GetGlyphRangesDefault()
    );

    if (sfProFont16) {
        this->pIO->FontDefault = sfProFont16;
        std::cout << "Шрифт SF Pro Text (16px) успешно загружен" << std::endl;
        if (sfProFont24) {
            std::cout << "Шрифт SF Pro Text (24px) также загружен" << std::endl;
        }
    } else {
        std::cerr << "Ошибка загрузки шрифта SF Pro Text" << std::endl;
        // Загружаем шрифт по умолчанию как fallback
        this->pIO->Fonts->AddFontDefault();
    }

    // Собираем атлас шрифтов
    this->pIO->Fonts->Build();
}

/**
 * @brief Initializes console output redirection
 * @tparam T Window type
 * @private
 * 
 * Sets up console output redirection by creating ConsoleRedirectBuffer
 * instances for stdout and stderr, and redirecting them to the ImGui console.
 */
template <typename T>
nil UserInterface<T>::__init_console_redirect() {
    // Создаем буферы для перенаправления
    cout_buffer = new ConsoleRedirectBuffer(static_cast<UserInterface<GLFWwindow>*>(this));
    cerr_buffer = new ConsoleRedirectBuffer(static_cast<UserInterface<GLFWwindow>*>(this));
    
    // Перенаправляем stdout и stderr в наш буфер
    std::cout.rdbuf(cout_buffer);
    std::cerr.rdbuf(cerr_buffer);
    
    // Добавляем приветственное сообщение
    __add_console_output("MentalEngine Console готов к работе");
    __add_console_output("Введите 'help' для списка команд");
}

/**
 * @brief Cleans up console output redirection
 * @tparam T Window type
 * @private
 * 
 * Restores original stdout and stderr streams and cleans up
 * ConsoleRedirectBuffer instances.
 */
template <typename T>
nil UserInterface<T>::__cleanup_console_redirect() {
    // Восстанавливаем стандартные потоки
    std::cout.rdbuf(nullptr);
    std::cerr.rdbuf(nullptr);
    
    // Удаляем буферы
    delete cout_buffer;
    delete cerr_buffer;
    cout_buffer = nullptr;
    cerr_buffer = nullptr;
}

/**
 * @brief Adds text to console output
 * @tparam T Window type
 * @param text Text to add to console
 * 
 * Thread-safe method to add text to the console output buffer.
 * Automatically handles line splitting and buffer size limits.
 */
template <typename T>
nil UserInterface<T>::__add_console_output(const std::string& text) {
    std::lock_guard<std::mutex> lock(console_mutex);
    
    // Разбиваем текст на строки
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        console_output.push_back(line);
        
        // Ограничиваем количество строк
        if (console_output.size() > MAX_CONSOLE_LINES) {
            console_output.erase(console_output.begin());
        }
    }
    
    console_scroll_to_bottom = true;
}

/**
 * @brief Implementation function for console output redirection
 * @tparam T Window type
 * @param ui Pointer to UserInterface instance
 * @param text Text to add to console
 * 
 * Helper function used by ConsoleRedirectBuffer to add text
 * to the console output in a thread-safe manner.
 */
template <typename T>
void __add_console_output_impl(void* ui, const std::string& text) {
    static_cast<UserInterface<T>*>(ui)->__add_console_output(text);
}

/**
 * @brief Checks if mouse is over the viewport area
 * @tparam T Window type
 * @return true if mouse is over viewport, false otherwise
 * 
 * This method checks if the current mouse position is within the bounds
 * of the viewport window. This is used to determine whether camera input
 * should be processed or if ImGui should handle the input.
 */
template <typename T>
bool UserInterface<T>::IsMouseOverViewport() {
    // Безопасная проверка - используем сохраненное состояние
    // вместо обращения к ImGui API, которое может быть недоступно
    return mouse_over_viewport;
}

/**
 * @brief Handles mouse input for drawing tools
 * @tparam T Window type
 * @param button Mouse button
 * @param action Button action (press/release)
 * @param x Mouse x coordinate
 * @param y Mouse y coordinate
 * 
 * Handles mouse button presses and releases for drawing operations.
 * Supports line drawing with left mouse button.
 */
template <typename T>
nil UserInterface<T>::HandleDrawingInput(int button, int action, float x, float y) {
    if (current_tool == ToolType::None) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // Start drawing
            is_drawing = true;
            // Безопасное преобразование координат мыши в координаты viewport
            // Используем размеры окна GLFW вместо ImGui API
            if (pWindow) {
                int window_width, window_height;
                glfwGetWindowSize(pWindow, &window_width, &window_height);
                
                if (window_width > 0 && window_height > 0) {
                    // Преобразуем координаты мыши в нормализованные координаты (-1 до 1)
                    float viewport_x = (x / window_width) * 2.0f - 1.0f;
                    float viewport_y = 1.0f - (y / window_height) * 2.0f;
                    
                    
                    line_start = MentalEngine::Math::Vector2(viewport_x, viewport_y);
                    line_end = MentalEngine::Math::Vector2(viewport_x, viewport_y);
                }
            }
        } else if (action == GLFW_RELEASE) {
            // Finish drawing
            if (is_drawing && current_tool == ToolType::Line) {
                // Add line to the list of drawn lines
                line_points.push_back(line_start);
                line_points.push_back(line_end);
            }
            is_drawing = false;
        }
    }
}

/**
 * @brief Handles mouse movement for drawing tools
 * @tparam T Window type
 * @param x Mouse x coordinate
 * @param y Mouse y coordinate
 * 
 * Handles mouse movement during drawing operations.
 * Updates the current line being drawn.
 */
template <typename T>
nil UserInterface<T>::HandleDrawingMouseMove(float x, float y) {
    (void)x; (void)y; // Suppress unused parameter warnings
    if (current_tool == ToolType::None || !is_drawing) return;
    
    // Безопасное преобразование координат мыши в координаты viewport
    // Используем размеры окна GLFW вместо ImGui API
    if (pWindow) {
        int window_width, window_height;
        glfwGetWindowSize(pWindow, &window_width, &window_height);
        
        if (window_width > 0 && window_height > 0) {
            // Преобразуем координаты мыши в нормализованные координаты (-1 до 1)
            float viewport_x = (x / window_width) * 2.0f - 1.0f;
            float viewport_y = 1.0f - (y / window_height) * 2.0f;
            
            // Update the end point of the current line being drawn
            line_end = MentalEngine::Math::Vector2(viewport_x, viewport_y);
        }
    }
}

#endif // MENTAL_USER_INTERFACE_H
