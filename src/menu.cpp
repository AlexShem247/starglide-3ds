#include "iostream"

#include "menu.h"
#include "keys.h"

#define TITLE_SIZE 0.15
#define TITLE_LEVEL 0.35
 
#define BTN_WIDTH 0.25
#define BTN_HEIGHT 0.2
#define BTN_LEVEL 0.575

#define BTN_TEXT_SIZE 0.07

int showMenu(GameEngine& gameEngine, GameResources& res, const string& titleText, const string& btnText,
            const string& btnScreenText, const string& message) {

    bool touchAlreadyHeld = gameEngine.getTouchHeldPosition().x != -1;

    // Draw starting screen
    while (gameEngine.gameIsRunning()) {
        double width = gameEngine.getScreenWidth();
        double height = gameEngine.getScreenHeight();

        // Check for input
        gameEngine.scanInput();
        vector<Key> keysPressed = gameEngine.getReleasedKeys();

        if (contains(keysPressed, PRIMARY_KEY))
            break;

        if (contains(keysPressed, START_KEY)) {
            gameEngine.terminateGame();
            break;
        }

        if (contains(keysPressed, SELECT_KEY)) {
            return -1;
        }

        Point touch = gameEngine.getTouchReleasedPosition();
        if (touch.x != -1) {
            if (!touchAlreadyHeld)
                break;
            touchAlreadyHeld = false;
        }

        // Draw menu
        gameEngine.startDrawing();
        gameEngine.clearBackground(COLOR_BLACK);
        gameEngine.drawImage(res.BG_IMAGE, { 0, 0 }, width, height);
        gameEngine.drawRect({ 0, 0 }, width, height, BLACK_TINT);
        gameEngine.drawText(res.TITLE_FONT, titleText, { 0.5 * width, TITLE_LEVEL * height }, true,
            TITLE_SIZE * height, 0.03 * width, COLOR_WHITE);

        double offset;
        if (message == "") {
            offset = 0;
        } else {
            gameEngine.drawText(res.BTN_FONT, message, { 0.5 * width, 0.55 * height }, true,
                BTN_TEXT_SIZE * height, 0.001 * width, COLOR_WHITE);
            offset = 0.15 * height;
        }
        gameEngine.drawRect({ (0.5 - BTN_WIDTH / 2) * width, BTN_LEVEL * height + offset }, BTN_WIDTH * width,
            BTN_HEIGHT * height, COLOR_BLUE);
        gameEngine.drawText(res.BTN_FONT, btnText, { 0.5 * width, 0.675 * height + offset }, true, BTN_TEXT_SIZE * height,
            0.001 * width, COLOR_WHITE);
        gameEngine.endDrawing();

        gameEngine.startDrawingLowerScreen();
        gameEngine.clearBackground(COLOR_BLACK);
        gameEngine.drawImage(res.BTN_BG_IMAGE, { 0, 0 }, width, height);
        gameEngine.drawText(res.BTN_FONT, btnScreenText,
        {0.4 * width, 0.2 * height}, true, 0.05 * height, 0.001 * width, COLOR_WHITE);
        if (message == "")
            gameEngine.drawText(res.BTN_FONT, "By Alexander Shemaly 2024",
                { 0.025 * width, 0.9 * height }, false, 0.05 * height, 0.001 * width, COLOR_WHITE);
        gameEngine.endDrawingLowerScreen();
    }

    return 0;
}