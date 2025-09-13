#ifndef MENTAL_T1_LAYER_H
#define MENTAL_T1_LAYER_H

#include "WindowManager/WindowManager.h"
#include <memory>
#include <GLFW/glfw3.h>
#include "../Core/Types.h"

class MentalT1Layer {
private:
    std::shared_ptr<class WindowManager<GLFWwindow>> ptrWindowManager = std::make_shared<WindowManager<GLFWwindow>>();
public:
    MentalT1Layer() {}
    inline nil Run() const { this->ptrWindowManager->Run(); }
    ~MentalT1Layer() {}
};



#endif // MENTAL_T1_LAYER_H
