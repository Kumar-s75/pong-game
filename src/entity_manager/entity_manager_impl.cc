#include "entity_manager.hpp"
#include <SDL2/SDL_events.h>
#include <memory>

void EntityManager::addEntity(const std::string& path, SDL_Renderer* renderer, Entity::Type t)
{
    // CREATE PLAYER
    if(t == Entity::Type::PLAYER) {
        int x = m_SCREEN_WIDTH / 2 - 25;
        int y = m_SCREEN_HEIGHT - 70;
        int width = 100;
        int height = 20;
        std::pair<float,float> speed = {0.15, 0};
        m_Entities.push_back(std::make_unique<Entity>(path, renderer, x, y, width, height, speed, Entity::Type::PLAYER));
    }
    // CREATE ENEMY
    else if(t == Entity::Type::ENEMY) {
        int x = m_SCREEN_WIDTH / 2 - 25;
        int y = 60;
        int width = 100;
        int height = 20;
        std::pair<float,float> speed = {0.15, 0};
        m_Entities.push_back(std::make_unique<Entity>(path, renderer, x, y, width, height, speed, Entity::Type::ENEMY));
    }
    // CREATE BALL
    else if(t == Entity::Type::BALL) {
        int x = m_SCREEN_WIDTH / 2;
        int y = m_SCREEN_HEIGHT / 2;
        int width = 30;
        int height = 30;
        std::pair<float,float> speed = {0.08, 0.08};
        m_Entities.push_back(std::make_unique<Entity>(path, renderer, x, y, width, height, speed, Entity::Type::BALL));
    }
}

void EntityManager::handleEvent(SDL_Event& event, std::shared_ptr<GameClient> client)
{
    for(auto& entity : m_Entities) {
        entity->handleEvent(event, client);

        if(entity->getType() == Entity::Type::BALL) continue;

        // Check for boundary conditions of player and enemy
        if(entity->getX() > m_SCREEN_WIDTH - entity->getWidth()) {
            entity->setX(m_SCREEN_WIDTH - entity->getWidth());
        }
        else if(entity->getX() <  0) {
            entity->setX(0);
        }
    }
}

void EntityManager::handleCollison(Sound* gameSound, bool& updateScore)
{
    for(auto& entity : m_Entities) {

        // Handle Ball - Wall Collision
        if(entity->getType() == Entity::Type::BALL) {
            bool playWallCollisionSound = false;

            // Check for boundary conditions
            if(entity->getX() > m_SCREEN_WIDTH )
            {
                entity->setVelocity({-entity->getVelocity().first, entity->getVelocity().second});
            }
            else if(entity->getX() < 0)
            {
                entity->setVelocity({-entity->getVelocity().first, entity->getVelocity().second});
            }
            else if(entity->getY() > m_SCREEN_HEIGHT)
            {
