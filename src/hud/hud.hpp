#ifndef HUD_HPP
#define HUD_HPP

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

class Hud
{
public:
    // Constructor
    Hud(SDL_Renderer* renderer, int& player, int& enemy)
    : m_Renderer(renderer),
    playerScore(player), enemyScore(enemy)
    {


    }

    void update()
    {

    }

    void render(float& deltaTime)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Create an ImGui window
