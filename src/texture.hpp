#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>

class Texture {
public:
    // Constructor for images
    Texture(const std::string& path, SDL_Renderer* renderer, int width, int height)
    : m_Texture(nullptr, SDL_DestroyTexture),
      m_Renderer(renderer),
      m_Width(width), m_Height(height)
    {
        SDL_Surface* surface = SDL_LoadBMP(path.c_str());
        if(surface == nullptr) {
            std::cerr << "Unable to load image! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Set the color key for the image
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF));

        // Create a texture from the surface
        m_Texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(SDL_CreateTextureFromSurface(renderer, surface), SDL_DestroyTexture);

        // Free the surface
        SDL_FreeSurface(surface);
