#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <array>

#include "shapes.h"
#include "gameConstants.h"

using namespace std;

// Maps a point with respect to a perspective point
Point transformPerspective(Point v, Point pp, double height);

// Returns the x coordinate given a vertical line index
double getLineXFromIndex(int i, Point pp, double width, double currentXOffset);

// Returns the y coordinate given a vertical line index
double getLineYFromIndex(int i, double height, double currentYOffset);

// Returns the current time in milliseconds
long long getCurrentTimeMillis();

// Generates a random integer between a and b inclusive
int getRandomInt(int a, int b);

// Returns bottom-left tile coordinates given its index
Point getTileCoordinates(int tX, int tY, Point pp, double width, double height,
    double currentXOffset, double currentYOffset, int currentYLoop);

// Checks if the ship has collided with a specified tile
bool checkShipCollisionWithTile(Point shipCenter, int tX, int tY,
    Point pp, double width, double height, double currentXOffset,
    double currentYOffset, int currentYLoop);

// Checks if the ship has collided with any tiles
bool checkShipCollision(const vector<Index2>& tiles, Point shipCenter, Point pp, double width, double height,
    double currentXOffset, double currentYOffset, int currentYLoop);

#endif // UTILS_H
