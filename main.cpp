#include "SDL2/SDL_error.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "SDL2/SDL_video.h"

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

class Screen {
public:

    SDL_Window* win;
    SDL_GLContext OpenGLcontext;

    std::vector<SDL_FPoint> points;

public:
    Screen(const std::string &vertexShaderIn, const std::string &fragmentShaderIn) {
        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        win = SDL_CreateWindow("test name", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

        OpenGLcontext = SDL_GL_CreateContext(win);
        if (!OpenGLcontext) {
            std::cout << "FAILED: create opengl contect" << SDL_GetError() << std::endl;
            class CreateRendererError{};
            throw CreateRendererError{};
        }

        if (!win) {
            std::cout << "FAILED: create window" << SDL_GetError() << std::endl;
            class CreateWindowError{};
            throw CreateWindowError{};
        }
        // Initialises GLAD
        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            std::cout << "FAILED: glad loading" << SDL_GetError() << std::endl;
            class LoadGladError{};
            throw LoadGladError{};
        }

        printOpenGLInfo();

        VertexSpecification();

        CreateGraphicsPipeline(vertexShaderIn, fragmentShaderIn);
    }

    void PreDraw() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(graphicsPipelineShaders);
    }
    
    void Draw() {
        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void add_point(float x, float y) {
        SDL_FPoint p{x, y};
        points.push_back(p);
    }

private:
    std::vector<GLfloat> tempTriangle{
        0.2, 0.2, 0.f,
        0.3, 0.1, 0.f,
        0.1, 0.1, 0.f
    };

    // NOTE: OpenGL objects
    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    GLuint graphicsPipelineShaders = 0;

    
    void VertexSpecification() {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, tempTriangle.size() * sizeof(GLfloat), tempTriangle.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)0);

        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
    }

    GLuint CompileShader(GLuint type, const std::string &src) {
        GLuint shaderObject;
        if (type == GL_VERTEX_SHADER) {
            shaderObject = glCreateShader(GL_VERTEX_SHADER);
        } else if (type == GL_FRAGMENT_SHADER) {
            shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        } else {
            shaderObject = 0;
        }

        const char* c_arr = src.c_str();
        glShaderSource(shaderObject, 1, &c_arr, nullptr);
        glCompileShader(shaderObject);

        return shaderObject;
    }

    void CreateGraphicsPipeline(const std::string &vertexShaderSrc, const std::string &fragmentShaderSrc) {
        GLuint programObject = glCreateProgram();
        GLuint loadedVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc);
        GLuint loadedFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

        glAttachShader(programObject, loadedVertexShader);
        glAttachShader(programObject, loadedFragmentShader);
        glLinkProgram(programObject);
        
        glValidateProgram(programObject);

        graphicsPipelineShaders = programObject;
    }



    void printOpenGLInfo() {
        std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    }
};



int main(int argc, char** argv){
    std::ifstream vs("VertexShader.glsl");
    std::stringstream vsSS;
    std::ifstream fs("FragmentShader.glsl");
    std::stringstream fsSS;
    if (vs && fs) {
        vsSS << vs.rdbuf();
        fsSS << fs.rdbuf();
        vs.close();
        fs.close();
    } else {
        std::cout << "FAILED: reading shaders" << std::endl;
        class fileReadError{};
        throw fileReadError{};
    }

    Screen screen(vsSS.str(), fsSS.str());

    // NOTE: Main loop
    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    SDL_DestroyWindow(screen.win);
                    break;

                default:
                    break;
            }
        }

        screen.PreDraw();
        screen.Draw();
        SDL_GL_SwapWindow(screen.win);
    }

    return 0;
}
