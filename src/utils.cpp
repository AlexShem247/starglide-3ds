#include <cstdlib>
#include <chrono>
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include "utils.h"

using namespace std;

Point transformPerspective(Point v, Point pp, double height) {
    if (PERPECTIVE_MODE) {
        // Invert the y-coordinate to match the top-left origin system
        double x = v.x;
        double y = height - v.y;
        pp.y = height - pp.y;

        // Calculate the transformed y coordinate
        double linY = min(y * pp.y / height, pp.y);

        // Calculate the transformed x coordinate
        double dX = x - pp.x;
        double dY = pp.y - linY;
        double scaleY = pow(dY / pp.y, 2);

        double trX = pp.x + scaleY * dX;
        double trY = pp.y - scaleY * pp.y;

        // Re-invert the y-coordinate before returning
        trY = height - trY;

        return { trX, trY };
    }
    else {
        return v;
    }
}

double getLineXFromIndex(int i, Point pp, double width, double currentXOffset) {
    double centreX = pp.x;
    double spacing = V_LINE_SPACING * width;
    double offset = i - 0.5;
    return centreX + offset * spacing + currentXOffset;
}

double getLineYFromIndex(int i, double height, double currentYOffset) {
    double spacingY = H_LINE_SPACING * height;
    return (NO_H_LINES - 1 - i) * spacingY + currentYOffset;
}

long long getCurrentTimeMillis() {
    // Get the current time point
    auto now = chrono::high_resolution_clock::now();

    // Convert time point to milliseconds
    auto millis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

    return millis;
}

int getRandomInt(int a, int b) {
    // Seed the random number generator with the current time
    srand((unsigned int)getCurrentTimeMillis());

    // Generate a random integer between a and b
    return a + (rand() % (b - a + 1));
}

Point getTileCoordinates(int tX, int tY, Point pp, double width, double height,
    double currentXOffset, double currentYOffset, int currentYLoop) {
    tY = tY - currentYLoop;
    Point p;
    p.x = getLineXFromIndex(tX, pp, width, currentXOffset);
    p.y = getLineYFromIndex(tY - 1, height, currentYOffset);
    return p;
}

bool checkShipCollisionWithTile(Point shipCenter, int tX, int tY,
    Point pp, double width, double height, double currentXOffset,
    double currentYOffset, int currentYLoop) {
    Point minP = getTileCoordinates(tX, tY, pp, width, height, currentXOffset,
        currentYOffset, currentYLoop);
    Point maxP = getTileCoordinates(tX + 1, tY + 1, pp, width, height, currentXOffset,
        currentYOffset, currentYLoop);

    return minP.x <= shipCenter.x && shipCenter.x <= maxP.x &&
        maxP.y <= shipCenter.y && shipCenter.y <= minP.y;
}

bool checkShipCollision(const vector<Index2>& tiles, Point shipCenter, Point pp, double width, double height,
    double currentXOffset, double currentYOffset, int currentYLoop) {
    for (int i = 0; i < (int)tiles.size(); i++) {
        Index2 tile = tiles[i];
        if (tile.y > currentYLoop + 1)
            return false;
        if (checkShipCollisionWithTile(shipCenter, tile.x, tile.y, pp, width, height,
            currentXOffset, currentYOffset, currentYLoop))
            return true;
    }
    return false;
}