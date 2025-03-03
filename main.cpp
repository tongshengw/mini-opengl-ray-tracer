#include "SDL2/SDL_error.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_stdinc.h"
#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "LinearAlgebra.hpp"
#include "SDL2/SDL_video.h"

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

class Camera {
private:
    v3<float> pos;
    Quaternion rotation = {std::array<float, 3>{0, 0, 1}, M_PI};
    float hFOV;
    float aspectRatio;
    float closePlane;
    float farPlane;

    v3<float> initial_orientation{0, 0, -1};
    v3<float> initial_right{1, 0, 0};

public:
    Camera()
        : pos({{0,0,0}}), hFOV(M_PI/4), aspectRatio(0.5625), closePlane(0.1), farPlane(100) {}

    Camera(v3<float>pos, Quaternion rotation, float hFOV, float closePlane, float farPlane, float aspectRatio)
        : pos(pos), rotation(rotation), hFOV(hFOV), aspectRatio(aspectRatio), closePlane(closePlane), farPlane(farPlane){}

    void move_forwards(float n) {
        v3<float> direction_vector = rotation.rotate_vector(initial_orientation);
        std::cout << direction_vector << std::endl;

        pos = pos + n * direction_vector;
    }
    void move_right(float n) {
        v3<float> direction_vector = rotation.rotate_vector(initial_right);

        pos = pos + n * direction_vector;
    }

    void local_rotate_y(float n) {
        v3<float> localY = rotation.rotate_vector({0,1,0});
        Quaternion q(localY, n);
        
        q.normalise();
        // TODO: no idea why using 2 q works lmfao figure this out and fix this
        rotation = rotation * q * q;
        rotation.normalise();
    }

    void local_rotate_x(float n) {
        v3<float> localX = rotation.rotate_vector({1,0,0});
        Quaternion q(localX, n);
        q.normalise();
        // TODO: no idea why using 2 q works lmfao figure this out and fix this
        rotation = rotation * q * q;
        rotation.normalise();
    }

    v3<float> get_pos() const {
        return pos;
    }
    
    v3<float> get_dir() const {
        return rotation.rotate_vector(initial_orientation);
    }

    m44<float> perspectiveMatrix() const {
        float f = farPlane;
        float n = closePlane;

        // This is equivalent to the OpenGL glFrustum matrix
        float l = -tan(hFOV) * closePlane;
        float r = tan(hFOV) * closePlane;
        float t = r*aspectRatio;
        float b = -r*aspectRatio;
        
        // double curly brackets because init an array, then init the m44
        return {
            {
                2*n/(r-l), 0, (r+l)/(r-l), 0,
                0, 2*n/(t-b), (t+b)/(t-b), 0,
                0, 0, -(f+n)/(f-n), -2*f*n/(f-n),
                0, 0, -1, 0
            }
        };
    }

    m44<float> viewMatrix() const {
        v3<float> xaxis{1, 0, 0};
        v3<float> yaxis{0, 1, 0};
        v3<float> zaxis{0, 0, 1};

        // This uses the conjugate as rotation represents the rotation of the camera, and we need to rotate world space points by the inverse and translate it in the opposite direction to convert into view space. https://www.songho.ca/opengl/gl_transform.html (Model-view matrix section)

        xaxis = rotation.conjugate().rotate_vector(xaxis);
        yaxis = rotation.conjugate().rotate_vector(yaxis);
        zaxis = rotation.conjugate().rotate_vector(zaxis);

        return {{
            xaxis.x, yaxis.x, zaxis.x, -pos.x,
            xaxis.y, yaxis.y, zaxis.y, -pos.y,
            xaxis.z, yaxis.z, zaxis.z, -pos.z,
            0, 0, 0, 1
        }};
    }

};

class Scene {

};



void GLAPIENTRY MessageCallback( GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

class Screen {
public:

    SDL_Window* win;
    SDL_GLContext OpenGLcontext;

    std::vector<SDL_FPoint> points;

    typedef struct Sphere {
        v3<float> location;
        float radius;
        v3<float> color;
        float emmission;
    } Sphere; 

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

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);

        printOpenGLInfo();

        VertexSpecification();

        CreateGraphicsPipeline(vertexShaderIn, fragmentShaderIn);
    }

    void PreDraw() {
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        glClearColor(0.0f, 0.0f, 1.0f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(graphicsPipelineShaders);

        GLint u_PerspectiveMatrixLocation = glGetUniformLocation(graphicsPipelineShaders, "u_PerspectiveMatrix");
        glUniformMatrix4fv(u_PerspectiveMatrixLocation, 1, GL_TRUE, camera.perspectiveMatrix().data.data());

        std::vector<Sphere> spheres;
        std::array<v4<float>, 3> worldCoordSpheres;
        worldCoordSpheres[0] = {0, 0, -10, 1};
        worldCoordSpheres[1] = {10, 0, -10, 1};
        worldCoordSpheres[2] = {0, -40, -20, 1};

        std::array<v3<float>, 3> sphereColors;
        sphereColors[0] = {1, 0, 0};
        sphereColors[1] = {0, 1, 0};
        sphereColors[2] = {0, 1, 1};

        std::array<float, 3> sphereRadii{1, 1, 30};
        std::array<float, 3> sphereEmmission{1, 0, 0};

        for (int i = 0; i < 3; i++) {
            v4<float> cameraCoordSphere = camera.viewMatrix() * worldCoordSpheres[i];
            spheres.push_back({{cameraCoordSphere}, sphereRadii[i], {sphereColors[i]}, sphereEmmission[i]});
        }

        for (int i = 0; i < 3; i++) {
            std::string locationStr =  "u_Spheres[" + std::to_string(i) + "].location";
            std::string radiusStr =  "u_Spheres[" + std::to_string(i) + "].radius";
            std::string colorStr =  "u_Spheres[" + std::to_string(i) + "].color";
            std::string emmissionStr = "u_Spheres[" + std::to_string(i) + "].emmission";

            GLint u_SpheresiLocationLocation = glGetUniformLocation(graphicsPipelineShaders, locationStr.c_str());
            GLint u_SpheresiRadiusLocation = glGetUniformLocation(graphicsPipelineShaders, radiusStr.c_str());
            GLint u_SpheresiColorLocation = glGetUniformLocation(graphicsPipelineShaders, colorStr.c_str());
            GLint u_SpheresiEmmissionLocation = glGetUniformLocation(graphicsPipelineShaders, emmissionStr.c_str());

            glUniform3f(u_SpheresiLocationLocation, spheres[i].location.x, spheres[i].location.y, spheres[i].location.z);
            glUniform1f(u_SpheresiRadiusLocation, spheres[i].radius);
            glUniform3f(u_SpheresiColorLocation, spheres[i].color.x, spheres[i].color.y, spheres[i].color.z);
            glUniform1f(u_SpheresiEmmissionLocation, spheres[i].emmission);
        }

        GLint u_RandSeedLocation = glGetUniformLocation(graphicsPipelineShaders, "u_RandSeed");
        int RandSeed = rand() * 100;
        glUniform1i(u_RandSeedLocation, RandSeed);
    }
    
    void Draw() {
        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
public:
    Camera camera;

private:
    std::vector<GLfloat> tempTriangle{
        -1.0f, -0.5625f, -1.0f,
        -1.0f, 0.5625f, -1.0f,
        1, -0.5625f, -1.0f,

        -1.0f, 0.5625f, -1.0f,
        1, -0.5625f, -1.0f,
        1.0f, 0.5625f, -1.0f
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
    std::ifstream vs("./shaders/vert.glsl");
    std::stringstream vsSS;
    std::ifstream fs("./shaders/frag.glsl");
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

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) {
            screen.camera.move_forwards(0.01f);
        }
        if (state[SDL_SCANCODE_DOWN]) {
            screen.camera.move_forwards(-0.01f);
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            screen.camera.move_right(0.01f);
        }
        if (state[SDL_SCANCODE_LEFT]) {
            screen.camera.move_right(-0.01f);
        }
        if (state[SDL_SCANCODE_D]) {
            screen.camera.local_rotate_y(0.001f);
        }
        if (state[SDL_SCANCODE_A]) {
            screen.camera.local_rotate_y(-0.001f);
        }
        if (state[SDL_SCANCODE_W]) {
            screen.camera.local_rotate_x(0.001f);
        }
        if (state[SDL_SCANCODE_S]) {
            screen.camera.local_rotate_x(-0.001f);
        }

        screen.PreDraw();
        screen.Draw();
        SDL_GL_SwapWindow(screen.win);
    }

    return 0;
}
