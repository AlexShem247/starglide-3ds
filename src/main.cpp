#include <iostream>

#include "game.h"
#include "menu.h"
#include "resources.h"

// Define which engine to use
#define USE_DESKTOP_ENGINE

#ifdef USE_DESKTOP_ENGINE
    #include "desktopEngine.h"
    using EngineType = DesktopEngine;
#else
    #include "n3dsEngine.h"
    using EngineType = N3DSEngine;
#endif

using namespace std;


int main() {
    EngineType gameEngine("STARGLIDE");

    // Load resouces
    GameResources res;
    res.BG_IMAGE = gameEngine.loadImage("assets/images/bg.png");
    res.SHIP_IMAGE = gameEngine.loadImage("assets/images/ship.png");
    res.BTN_BG_IMAGE = gameEngine.loadImage("gfx/lowerBg.png");
    res.TITLE_FONT = gameEngine.loadFont("assets/fonts/Pirulen.ttf");
    res.BTN_FONT = gameEngine.loadFont("assets/fonts/Zekton.ttf");

    int back;

    while (gameEngine.gameIsRunning()) {
        // Show starting menu
        back = showMenu(gameEngine, res, "STARGLIDE", "START", "Press A or tap the screen to Start", "");

        // Start the game
        while (gameEngine.gameIsRunning() && back != -1) {
            int score = startGame(gameEngine, res);
            back = showMenu(gameEngine, res, "GAME OVER", "RESTART", "Press A or tap the screen to Play Again",
                "Your score was: " + to_string(score));
        }
    }
    
    gameEngine.freeResources();
        
    return 0;
}
