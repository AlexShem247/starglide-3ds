#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>

#include "keys.h"
#include "colors.h"
#include "shapes.h"

using namespace std;

class GameEngine {
public:
    // Constructor
    GameEngine(const char* title);

    // Virtual destructor
    virtual ~GameEngine();

    // Pure virtual methods to be implemented by derived classes
    virtual bool gameIsRunning() = 0;
    virtual void startDrawing() = 0;
    virtual void clearBackground(RGB_Color color) = 0;
    virtual void drawRect(Point p, double width, double height, RGB_Color fill) = 0;
    virtual void drawLine(Point start, Point end, RGB_Color color) = 0;
    virtual void drawTriangle(Point p1, Point p2, Point p3, RGB_Color fill) = 0;
    virtual void drawQuad(Point p1, Point p2, Point p3,Point p4, RGB_Color fill) = 0;
    virtual void endDrawing() = 0;
    virtual void freeResources() = 0;
    virtual void terminateGame() = 0;
    virtual void startDrawingLowerScreen() = 0;
    virtual void endDrawingLowerScreen() = 0;

    // Loads an image into the game engine and returns an id for drawing
    virtual int loadImage(const string& filename) = 0;
    // Draws an image given its id
    virtual void drawImage(int id, Point p, double width, double height) = 0;

    // Loads a font into the game engine and returns an id for drawing
    virtual int loadFont(const string& filename) = 0;
    // Draws text given a string and a font
    virtual void drawText(int id, const string& text, Point p, bool center, double fontSize, double spacing,
                          RGB_Color color) = 0;

    virtual void scanInput() = 0;
    virtual const vector<Key> getReleasedKeys() = 0;
    virtual const vector<Key> getHeldKeys() = 0;
    virtual Point getTouchHeldPosition() = 0;
    virtual Point getTouchReleasedPosition() = 0;
    virtual Point getTouchDragged() = 0;

    virtual double getDeltaTime() = 0;
    virtual int getScreenWidth() = 0;
    virtual int getScreenHeight() = 0;

protected:
    const char* title;
};

#endif // GAMEENGINE_H
