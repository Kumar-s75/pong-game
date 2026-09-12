#include "game.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <memory>

Game::Game(SDL_Window* window, SDL_Renderer* renderer, Sound* sound, std::shared_ptr<GameClient> client)
    : m_Window(window), m_Renderer(renderer), m_GameSound(sound), m_Client(client)
{
    init();
    m_GameMode = GameMode::OFFLINE;
}

Game::~Game() {
    clean();
    //SDL_Quit();
}

void Game::init() {

    // Init Background Texture
    m_BackgroundTexture = std::make_unique<Texture>("assets/background.bmp", m_Renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Initialize the Entity Manager
    m_EntityManager = std::make_unique<EntityManager>(SCREEN_WIDTH, SCREEN_HEIGHT, playerScore, enemyScore);

    // Initialize the HUD
    m_Hud = std::make_unique<Hud>(m_Renderer, playerScore, enemyScore);

    addEntity("assets/blue.bmp", Entity::Type::PLAYER);
    addEntity("assets/red.bmp", Entity::Type::ENEMY);
    addEntity("assets/ball.bmp", Entity::Type::BALL);

}

void Game::handleEvents(SDL_Event& event) {
    // Handling In game events
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym)
        {
            default:
                break;
        }
    }
    // Pass the event to the entity manager
    m_EntityManager->handleEvent(event, m_Client);
}

void Game::update() {

    // handle Collision
    m_EntityManager->handleCollison(m_GameSound, updateScore);

    // Update Huds
    if(updateScore)
    {
        updateScore = false;
        m_Client->SendScore(playerScore, enemyScore);
        std::cout << "PLAYER: " << playerScore << "\tENEMY: " << enemyScore << std::endl;
        if(playerScore == 10 || enemyScore == 10) {
            if(playerScore == 10) {
                std::cout << "Player Wins!" << std::endl;
            }
            else {
                std::cout << "Enemy Wins!" << std::endl;
            }
            m_Client->SendReset();
            resetGame();
        }
    }

    // Get Data From Server
    if(m_Client && m_Client->IsConnected() && !m_Client->Incoming().empty()) {
        auto msg = m_Client->Incoming().pop_front().msg;
        InternalMessageType mess = m_Client->OnMessage(msg);

        // Update Score
        if(mess.id == GameMsg::Score_Update) {
            int player, enemy;
            ParseScoreUpdate(mess.message, player, enemy);
            // Reverse the scores
            playerScore = enemy;
            enemyScore = player;
        }

        // Update Enemy Position
        else if(mess.id == GameMsg::Enemy_Update) {
            float xPos, yPos;
            ParseEnemyUpdateMessage(mess.message, xPos, yPos);
            TransformEnemyCoordinates(xPos, yPos);
            m_EntityManager->setEnemyPos(xPos, yPos);
        }

        // Update Ball Position and Velocity
        else if(mess.id == GameMsg::Ball_Update) {
            float xPos, yPos, xVel, yVel;
            ParseBallUpdateMessage(mess.message, xPos, yPos, xVel, yVel);
