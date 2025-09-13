#ifndef MENTAL_RENDERER_H
#define MENTAL_RENDERER_H

#include <GL/glew.h>
#include "../../Core/Types.h"
#include <functional>

class Renderer {
  public:
    Renderer() {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {return;}
    }
    nil DrawFrame(std::function<nil()> operation) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        operation();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    }
};

#endif // MENTAL_REDENRER_H
