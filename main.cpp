#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>

#include "LinearAlgebra.hpp"

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

class Screen {
public:
    Screen() {
        SDL_Init(SDL_INIT_EVERYTHING);
        win = SDL_CreateWindow("test name", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            class CreateRendererError{};
            throw CreateRendererError{};
        }

        if (!win) {
            std::cout << "FAILED: create window" << SDL_GetError() << std::endl;
            class CreateWindowError{};
            throw CreateWindowError{};
        }

        
    }

    void add_point(float x, float y) {
        SDL_FPoint p{x, y};
        points.push_back(p);
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        Triangle test({100, 100, 0}, {500, 500, 0}, {100, 300, 0});
        drawTriangle(test);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (SDL_FPoint p : points) {
            SDL_RenderDrawPointF(renderer, p.x, p.y);
        }

        points.clear();

        SDL_RenderPresent(renderer);
    } 
    
private:
    void drawTriangle(Triangle t) {
        drawLine(t.a.x, t.a.y, t.b.x, t.b.y);
        drawLine(t.b.x, t.b.y, t.c.x, t.c.y);
        drawLine(t.a.x, t.a.y, t.c.x, t.c.y);
    }

    void drawLine(float x1, float y1, float x2, float y2) {
        if (std::abs(y2-y1) < std::abs(x2-x1)) {
            if (x1 > x2) {
                drawLineLow(x2, y2, x1, y1);
            } else {
                drawLineLow(x1, y1, x2, y2);
            }
        } else {
            if (y1 > y2) {
                drawLineHigh(x2, y2, x1, y1);
            } else {
                drawLineHigh(x1, y1, x2, y2);
            }
        }
    }

    void drawLineLow(float x1, float y1, float x2, float y2) {
        float dx = x2-x1;
        float dy = y2-y1;
        float yi = 1;
        if (dy < 0) {
            yi = -1;
            dy = -dy;
        }
        float D = 2*dy - dx;
        float y = y1;

        for (int i = (int) x1; i < x2; i++) {
            add_point(i, y);
            if (D>0) {
                y = y+yi;
                D = D + (2*(dy-dx));
            } else {
                D = D + 2*dy;
            }
        }
    }

    void drawLineHigh(float x1, float y1, float x2, float y2) {
        float dx = x2-x1;
        float dy = y2-y1;
        float xi = 1;
        if (dx < 0) {
            xi = -1;
            dx = -dx;
        }
        float D = 2*dx - dy;
        float x = x1;

        for (int i = (int) y1; i < y2; i++) {
            add_point(x, i);
            if (D>0) {
                x = x+xi;
                D = D + (2*(dx-dy));
            } else {
                D = D + 2*dx;
            }
        }
    }
    SDL_Window* win;
    SDL_Renderer* renderer;

    std::vector<SDL_FPoint> points;
    
};


class Triangles {
public:

    int size() {
        return triangles.size();
    }

    void add_triangle(v3<float> a, v3<float> b, v3<float> c) {
        bool aFound = false;
        bool bFound = false;
        bool cFound = false;
        int outA;
        int outB;
        int outC;

        int i = 0;
        while ((!aFound || !bFound || !cFound) && i < (int) vertices.size()) {
            if (!aFound && vertices[i] == a) {
                outA = i;
            }
            if (!bFound && vertices[i] == b) {
                outB = i;
            }
            if (!cFound && vertices[i] == c) {
                outC = i;
            }
        }
        if (aFound && bFound && cFound) {
            std::array<int, 3> output{outA, outB, outC};
            triangles.push_back(output);
        } else {
            int counter = 0;
            if (!aFound) {
                outA = vertices.size() + counter;
                vertices.push_back(a);
                counter++;
            }
            if (!bFound) {
                outB = triangles.size() + counter;
                vertices.push_back(b);
                counter++;
            }
            if (!cFound) {
                outC = triangles.size() + counter;
                vertices.push_back(c);
                counter++;
            }
            std::array<int, 3> output{outA, outB, outC};
            triangles.push_back(output);
        }
    }

    Triangle at(int i) {
        if (i >= (int) triangles.size() || i < 0) {
            class TrianglesIndexPastEndError{};
            throw TrianglesIndexPastEndError{};
        } else {
            Triangle output(vertices[triangles[i][0]], vertices[triangles[i][1]], vertices[triangles[i][2]]);

            return output;
        }
    }

private:
    std::vector<v3<float>> vertices;

    // triagles use the index of each vertex in the vertices vector. 
    std::vector<std::array<int, 3>> triangles;
};

int main(int argc, char** argv){
    
    Screen screen;

    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }

        screen.render();

    }

    return 0;
}
