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

// Forward declaration для метода
template <typename T> void __add_console_output_impl(void* ui, const std::string& text);

// Класс для перехвата вывода в консоль
class ConsoleRedirectBuffer : public std::streambuf {
private:
    void* ui;
    
public:
    ConsoleRedirectBuffer(void* ui) : ui(ui) {}
    
protected:
    virtual int_type overflow(int_type c) override {
        if (c != EOF) {
            char ch = static_cast<char>(c);
            __add_console_output_impl<GLFWwindow>(ui, std::string(1, ch));
        }
        return c;
    }
    
    virtual std::streamsize xsputn(const char* s, std::streamsize count) override {
        __add_console_output_impl<GLFWwindow>(ui, std::string(s, count));
        return count;
    }
};

template <typename T = GLFWwindow>
class UserInterface
{
private:
    T* pWindow = nullptr;
    ImGuiContext *pCTX = nullptr;
    ImGuiIO *pIO = nullptr;
    class Renderer* pRenderer = nullptr;

    bool show_demo_window = true;

    // Терминал
    std::vector<std::string> console_output;
    std::string console_input;
    bool console_scroll_to_bottom = true;
    std::mutex console_mutex;
    static const size_t MAX_CONSOLE_LINES = 1000;
    ConsoleRedirectBuffer* cout_buffer = nullptr;
    ConsoleRedirectBuffer* cerr_buffer = nullptr;

    nil __load_default_fonts();
    nil __init_console_redirect();
    nil __cleanup_console_redirect();

public:
    nil __add_console_output(const std::string& text);
    UserInterface(T* pWindow, class Renderer* pRenderer);
    nil NewFrame();
    nil EndFrame();
    nil Dockspace();
    nil MainMenu();
    nil DrawFrame();
    nil Viewport();
    nil Toolbox();
    nil Console(); 
    ~UserInterface();
};

template <typename T>
nil UserInterface<T>::Toolbox() {
    ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus);
    ImGui::Text("Toolbox");
    ImGui::End();
}

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

template <typename T>
nil UserInterface<T>::Viewport() {
    ImGui::Begin("Viewport");
    
    if (pRenderer) {
        // Получаем размер доступной области
        ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
        int width = static_cast<int>(viewport_panel_size.x);
        int height = static_cast<int>(viewport_panel_size.y);
        
        // Рендерим viewport через Renderer
        pRenderer->RenderViewport(width, height);
        
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

template <typename T>
nil UserInterface<T>::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

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
    return;
}

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

// Реализация функции для перехвата вывода
template <typename T>
void __add_console_output_impl(void* ui, const std::string& text) {
    static_cast<UserInterface<T>*>(ui)->__add_console_output(text);
}

#endif // MENTAL_USER_INTERFACE_H
