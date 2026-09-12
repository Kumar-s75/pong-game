#ifndef MENU_HPP
#define MENU_HPP

#include "../texture.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <memory>

class Menu
{
private:
    static const int SCREEN_WIDTH = 1280;
    static const int SCREEN_HEIGHT = 720;

public:
    Menu(SDL_Window* window, SDL_Renderer* renderer)
        : m_Window(window), m_Renderer(renderer)
    {
        // Init Background Texture
        m_MenuTexture = std::make_unique<Texture>("assets/main_menu.bmp", m_Renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        m_LogoTexture = std::make_unique<Texture>("assets/barricade-blitz.bmp", m_Renderer, (SCREEN_WIDTH/10)*6, (SCREEN_HEIGHT/10)*6);

    }

    void handleEvents(SDL_Event& event)
    {

    }

