#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>

#include "LinearAlgebra.hpp"
#include "Triangles.hpp"

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

class Screen {
private:

    SDL_Window* win;
    SDL_Renderer* renderer;

    std::vector<SDL_FPoint> points;

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
        std::unordered_map<int, std::pair<int, int>> edgePoints;
        drawLine(edgePoints, t.a.x, t.a.y, t.b.x, t.b.y);
        drawLine(edgePoints, t.b.x, t.b.y, t.c.x, t.c.y);
        drawLine(edgePoints, t.a.x, t.a.y, t.c.x, t.c.y);

        std::unordered_map<int, std::pair<int, int>> throwaway;
        for (std::pair<int, std::pair<int, int>> pair : edgePoints) {
            if (pair.second.first != -1 && pair.second.second != -1) {
                drawLine(throwaway, pair.first, pair.second.first, pair.first, pair.second.second);
            }
        }
    }

    void drawLine(std::unordered_map<int, std::pair<int, int>> &edgePoints, float x1, float y1, float x2, float y2) {
        if (std::abs(y2-y1) < std::abs(x2-x1)) {
            if (x1 > x2) {
                drawLineLow(edgePoints, x2, y2, x1, y1);
            } else {
                drawLineLow(edgePoints, x1, y1, x2, y2);
            }
        } else {
            if (y1 > y2) {
                drawLineHigh(edgePoints, x2, y2, x1, y1);
            } else {
                drawLineHigh(edgePoints, x1, y1, x2, y2);
            }
        }
    }

    void drawLineLow(std::unordered_map<int, std::pair<int, int>> &edgePoints, float x1, float y1, float x2, float y2) {
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
            if (edgePoints.find(i) == edgePoints.end()) {
                std::pair<int, int> p{y, -1};
                edgePoints[i] = p;
            } else {
                edgePoints[i].second = y;
            }
            if (D>0) {
                y = y+yi;
                D = D + (2*(dy-dx));
            } else {
                D = D + 2*dy;
            }
        }
    }

    void drawLineHigh(std::unordered_map<int, std::pair<int, int>> &edgePoints, float x1, float y1, float x2, float y2) {
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
            if (edgePoints.find(x) == edgePoints.end()) {
                std::pair<int, int> p{i, -1};
                edgePoints[x] = p;
            } else {
                edgePoints[x].second = i;
            }
            if (D>0) {
                x = x+xi;
                D = D + (2*(dx-dy));
            } else {
                D = D + 2*dx;
            }
        }
    }
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
