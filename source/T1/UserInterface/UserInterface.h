#ifndef MENTAL_USER_INTERFACE_H
#define MENTAL_USER_INTERFACE_H

#include <iostream>
#include <cstdio>

#include "../../Core/Types.h"
#include "../Renderer/Renderer.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

template <typename T = GLFWwindow>
class UserInterface
{
private:
    T* pWindow = nullptr;
    ImGuiContext *pCTX = nullptr;
    ImGuiIO *pIO = nullptr;
    class Renderer* pRenderer = nullptr;

    bool show_demo_window = true;

    nil __load_default_fonts();

public:
    UserInterface(T* pWindow, class Renderer* pRenderer);
    nil NewFrame();
    nil EndFrame();
    nil Dockspace();
    nil MainMenu();
    nil DrawFrame();
    nil Viewport();
    nil Toolbox(); 
    ~UserInterface();
};

template <typename T>
nil UserInterface<T>::Toolbox() {
    ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus);
    ImGui::Text("Toolbox");
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

    // Окно консоли
    ImGui::Begin("Console");
    ImGui::Text("Добро пожаловать в MentalEngine!");
    ImGui::Text("Dockspace активен - перетаскивайте окна для настройки макета");
    ImGui::Separator();
    static char input_buf[256] = "";
    ImGui::InputText("Command", input_buf, sizeof(input_buf));
    ImGui::SameLine();
    if (ImGui::Button("Send")) {
        // Логика отправки команды
        input_buf[0] = '\0';
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

    this->pWindow = pWindow;
    this->pRenderer = pRenderer;
    return;
}

template <typename T>
UserInterface<T>::~UserInterface() {
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

#endif // MENTAL_USER_INTERFACE_H
