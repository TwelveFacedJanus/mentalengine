#ifndef MENTAL_USER_INTERFACE_H
#define MENTAL_USER_INTERFACE_H

#include <GLFW/glfw3.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>

#include "../../Core/Types.h"

template <typename T = GLFWwindow>
class UserInterface
{
private:
    T* pWindow = nullptr;
    ImGuiContext *pCTX = nullptr;
    ImGuiIO *pIO = nullptr;

    bool show_demo_window = true;

    nil __load_default_fonts();

public:
    UserInterface(T* pWindow);
    nil NewFrame();
    nil EndFrame();
    nil Dockspace();
    nil MainMenu();
    nil DrawFrame();
    ~UserInterface();
};

template <typename T>
nil UserInterface<T>::DrawFrame() {
    this->NewFrame();
    this->Dockspace();
    this->MainMenu();

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
UserInterface<T>::UserInterface(T* pWindow) {
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
