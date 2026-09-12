#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "../texture.hpp"
#include "./../net/client.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <memory>


class Entity {
private:
    static const int m_SCREEN_WIDTH = 1280;
    static const int m_SCREEN_HEIGHT = 720;
public:
    // Entity Type
    enum class Type {
        PLAYER,
        ENEMY,
        BALL
    };

    Entity(const std::string& path, SDL_Renderer* renderer, int x, int y, int w, int h, std::pair<float,float> s, enum Type t)
    : m_Texture(nullptr),
      m_Xpos(x), m_Ypos(y),
      m_Xspeed(s.first), m_Yspeed(s.second),
      m_Xvel(0), m_Yvel(0),
      m_Alive(true),
      m_Type(t)
    {
        m_Texture = std::make_unique<Texture>(path, renderer, w, h);
        if(t == Entity::Type::BALL) {
            m_Xvel = m_Xspeed;
            m_Yvel = m_Yspeed;
        }
    }

    ~Entity() {}

    void handleEvent(SDL_Event& event, std::shared_ptr<GameClient> client);
