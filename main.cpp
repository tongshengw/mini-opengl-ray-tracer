#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <array>

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
        for (int x = 0; x < 1000; x++) {
            for (int y = 0; y < 1000; y++) {
                add_point(x, y);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (SDL_FPoint p : points) {
            SDL_RenderDrawPointF(renderer, p.x, p.y);
        }

        points.clear();

        SDL_RenderPresent(renderer);
    } 
    
private:
    SDL_Window* win;
    SDL_Renderer* renderer;

    std::vector<SDL_FPoint> points;
    
};


class Triangles {
public:

    void add_triangle(v3<float> a, v3<float> b, v3<float> c) {
        bool aFound = false;
        bool bFound = false;
        bool cFound = false;
        int outA;
        int outB;
        int outC;

        int i = 0;
        while ((!aFound || !bFound || !cFound) && i < vertices.size()) {
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
            v3<int> output{outA, outB, outC};
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
            v3<int> output{outA, outB, outC};
            triangles.push_back(output);
        }
    }

    std::array<v3<float>, 3> at(int i) {
        if (i >= triangles.size() || i < 0) {
            class TrianglesIndexPastEndError{};
            throw TrianglesIndexPastEndError{};
        } else {
            std::array<v3<float>, 3> output;
            output[0] = vertices[triangles[i].x];
            output[1] = vertices[triangles[i].y];
            output[2] = vertices[triangles[i].z];

            return output;
        }
    }

private:
    std::vector<v3<float>> vertices;

    // triagles use the index of each vertex in the vertices vector. 
    std::vector<v3<int>> triangles;
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
