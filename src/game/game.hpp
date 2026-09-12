#ifndef GAME_HPP
#define GAME_HPP

#include "../entity_manager/entity_manager.hpp"
#include "../entity/entity.hpp"
#include "../hud/hud.hpp"
#include "../sound/sound.hpp"

#include "../net/client.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>


class Game{
private:
    static const int SCREEN_WIDTH = 1280;
    static const int SCREEN_HEIGHT = 720;

public:

    // GAME MODE TYPE
    enum class GameMode {
        OFFLINE,
        ONLINE
    };

    Game(SDL_Window* window, SDL_Renderer* renderer, Sound* sound, std::shared_ptr<GameClient> client);
    ~Game();

    void init();
    void run();
    void quit();
    void render(float& deltaTime);
    void update();
    void handleEvents(SDL_Event& event);
    void clean();
    bool running() { return isGameRunning; }

    void setRunning(bool flag) { isGameRunning = flag; }
    void resetGame();
