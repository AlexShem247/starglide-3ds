#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <array>

#include "game.h"
#include "keys.h"
#include "colors.h"
#include "shapes.h"
#include "gameConstants.h"
#include "utils.h"

int startGame(GameEngine& gameEngine, GameResources& res) {
    double currentXOffset = 0;
    double currentYOffset = 0;
    int currentYLoop = 0;
    double speedY = SPEED_Y;

    // Add initial tiles
    vector<Index2> tiles;
    Index2 p;
    for (int i = 0; i < NO_STARTING_TILES; i++) {
        p = { 0, i };
        tiles.push_back(p);
    }

    gameEngine.getDeltaTime();

    // Main game loop
    while (gameEngine.gameIsRunning()) {
        double dt = gameEngine.getDeltaTime();
        double width = gameEngine.getScreenWidth();
        double height = gameEngine.getScreenHeight();

        double perspectivePointX = width * 0.5;
        double perspectivePointY = height * 0.25;

        Point pPoint = { perspectivePointX, perspectivePointY };

        // Draw shapes
        gameEngine.startDrawing();
        gameEngine.clearBackground(COLOR_BLACK);
        gameEngine.drawImage(res.BG_IMAGE, { 0, 0 }, width, height);

        // Calculate vertical line offset
        gameEngine.scanInput();
        
        vector<Key> keysPressed = gameEngine.getReleasedKeys();
        if (contains(keysPressed, START_KEY)) {
            gameEngine.terminateGame();
            break;
        }

        vector<Key> keysHeld = gameEngine.getHeldKeys();

        // Moving using keys/buttons
        if (contains(keysHeld, LEFT_KEY))
            currentXOffset += width * SPEED_X * dt;
        if (contains(keysHeld, RIGHT_KEY))
            currentXOffset -= width * SPEED_X * dt;

        // Moving using touchscreen
        Point touch;
        if (RELATIVE_SLIDE_MODE) {
            // Move by sliding touchscreen
            touch = gameEngine.getTouchDragged();
            currentXOffset -= SLIDE_SCALE * width * touch.x * dt;

        }
        else {
            // Each point on touchscreen is mapped to currentXOffset
            touch = gameEngine.getTouchHeldPosition();
            if (touch.x != -1)
                currentXOffset = getLineXFromIndex(NO_V_LINES / 2, pPoint, width,
                    -2 * (V_LINE_SPACING * width) * (((NO_V_LINES / 2) - 0.5) * touch.x + 1));
        }

        // Draw vertical lines
        int startIndex = -(NO_V_LINES / 2) + 1;
        int endIndex = startIndex + NO_V_LINES - 1;
        for (int i = startIndex; i < startIndex + NO_V_LINES; i++) {
            double x = getLineXFromIndex(i, pPoint, width, currentXOffset);
            Point start = transformPerspective({ x, 0 }, pPoint, height);
            Point end = transformPerspective({ x, height }, pPoint, height);

            gameEngine.drawLine(start, end, COLOR_WHITE);
        }


        // Draw horizontal lines
        double xMin = getLineXFromIndex(startIndex, pPoint, width, currentXOffset);
        double xMax = getLineXFromIndex(endIndex, pPoint, width, currentXOffset);


        // Calculate horizontal line offset
        speedY += dt * SPEED_Y_INC_PER_SND;
        currentYOffset += speedY * height * dt;

        double spacingY = H_LINE_SPACING * height;
        while (currentYOffset >= spacingY) {
            currentYOffset -= spacingY;
            currentYLoop += 1;
        }

        for (int i = 0; i < NO_H_LINES; i++) {
            double lineY = getLineYFromIndex(i, height, currentYOffset);
            Point start = transformPerspective({ xMin, lineY }, pPoint, height);
            Point end = transformPerspective({ xMax, lineY }, pPoint, height);

            gameEngine.drawLine(start, end, COLOR_WHITE);
        }

        // 1. Generate tiles
        int lastX = 0, lastY = 0;

        // Clean the tiles that are out of the screen
        for (int i = (int)tiles.size() - 1; i >= 0; i--) {
            if (tiles[i].y < currentYLoop) {
                tiles.erase(tiles.begin() + i);
            }
        }

        if (tiles.size() > 0) {
            Index2 lastTile = tiles.back();
            lastX = lastTile.x;
            lastY = lastTile.y + 1;
        }

        // Add new tiles if there is space
        for (int i = (int)tiles.size(); i <= NO_TILES; i++) {
            int r = getRandomInt(0, 2);
            if (lastX <= startIndex)
                r = 1;
            if (lastX >= endIndex - 1)
                r = 2;

            p = { lastX, lastY };
            tiles.push_back(p);

            if (r == 1) {
                // Path moves to the right
                lastX++;
                p = { lastX, lastY };
                tiles.push_back(p);

                lastY++;
                p = { lastX, lastY };
                tiles.push_back(p);

            }
            else if (r == 2) {
                // Path moves to the left
                lastX--;
                p = { lastX, lastY };
                tiles.push_back(p);

                lastY++;
                p = { lastX, lastY };
                tiles.push_back(p);
            }
            lastY++;
        }

        // 2. Draw tiles
        for (int i = 0; i < NO_TILES; i++) {
            Index2 tile = tiles[i];
            Point pMin = getTileCoordinates(tile.x, tile.y, pPoint, width, height,
                currentXOffset, currentYOffset, currentYLoop);
            Point pMax = getTileCoordinates(tile.x + 1, tile.y + 1, pPoint, width, height,
                currentXOffset, currentYOffset, currentYLoop);

            Point p1 = transformPerspective({ pMin.x, pMin.y }, pPoint, height);
            Point p2 = transformPerspective({ pMin.x, pMax.y }, pPoint, height);
            Point p3 = transformPerspective({ pMax.x, pMax.y }, pPoint, height);
            Point p4 = transformPerspective({ pMax.x, pMin.y }, pPoint, height);
            gameEngine.drawQuad(p1, p2, p3, p4, COLOR_WHITE);
        }

        // Draw ship
        double centreX = width / 2;
        double baseY = height - SHIP_BASE_Y * height;
        double shipHalfWidth = SHIP_WIDTH * width / 2;
        double shipHeight = SHIP_HEIGHT * height;

         /*Point p1 = transformPerspective({ centreX - shipHalfWidth, baseY }, pPoint, height);
         Point p2 = transformPerspective({ centreX, baseY - shipHeight }, pPoint, height);
         Point p3 = transformPerspective({ centreX + shipHalfWidth, baseY }, pPoint, height);

         gameEngine.drawTriangle(p1, p2, p3, COLOR_BLACK);*/

        // Check if ship is out of bounds
        Point shipCenter = { centreX, baseY - shipHeight / 2 };
        if (!checkShipCollision(tiles, shipCenter, pPoint, width, height,
            currentXOffset, currentYOffset, currentYLoop)) {
            break;
        }

        gameEngine.drawImage(res.SHIP_IMAGE, { centreX - shipHalfWidth, baseY - shipHeight * 2.5 },
            shipHalfWidth * 2.5, shipHeight * 2.5);

        // Display score
        gameEngine.drawText(res.BTN_FONT, "Score: " + to_string(currentYLoop + 1),
            { 0.025 * width, 0.05 * height }, false, 0.07 * height, 0.001 * width, COLOR_WHITE);

        gameEngine.endDrawing();

        gameEngine.startDrawingLowerScreen();
        gameEngine.clearBackground(COLOR_BLACK);
        gameEngine.drawImage(res.BTN_BG_IMAGE, { 0, 0 }, width, height);
        gameEngine.drawText(res.BTN_FONT, "Use the Circle Pad or slide the touchscreen\nto move the Ship",
        {0.4 * width, 0.2 * height}, true, 0.05 * height, 0.001 * width, COLOR_WHITE);
        gameEngine.endDrawingLowerScreen();
    }

    return currentYLoop + 1;
}