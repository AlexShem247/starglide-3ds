#ifndef DESKTOPENGINE_H
#define DESKTOPENGINE_H

#include <raylib.h>
#include <string>
#include <vector>

#include "gameEngine.h"
#include "shapes.h"


using namespace std;

class DesktopEngine : public GameEngine {
public:
    // Constructor
    DesktopEngine(const char* title);

    // Destructor
    virtual ~DesktopEngine();

    // Implement the pure virtual methods from GameEngine
    bool gameIsRunning();
    void startDrawing();
    void clearBackground(RGB_Color color);
    void drawRect(Point p, double width, double height, RGB_Color fill);
    void drawLine(Point start, Point end, RGB_Color color);
    void drawTriangle(Point p1, Point p2, Point p3, RGB_Color fill);
    void drawQuad(Point p1, Point p2, Point p3, Point p4, RGB_Color fill);
    void endDrawing();
    void freeResources();
    void startDrawingLowerScreen();
    void endDrawingLowerScreen();
    void terminateGame();

    int loadImage(const string& filename);
    void drawImage(int id, Point p, double width, double height);

    int loadFont(const string& filename);
    void drawText(int id, const string& text, Point p, bool center,
        double fontSize, double spacing, RGB_Color color);

    void scanInput();
    const vector<Key> getReleasedKeys();
    const vector<Key> getHeldKeys();
    Point getTouchHeldPosition();
    Point getTouchReleasedPosition();
    Point getTouchDragged();

    double getDeltaTime();
    int getScreenWidth();
    int getScreenHeight();

private:
    bool drawing = true;
    vector<Texture2D> textures;
    vector<Font> fonts;

    const vector<Key> calcHeldKeys();
    vector<Key> heldKeys;
    vector<Key> releasedKeys;
    bool wasMousePressed = false;
    bool gameIsTerminated = false;
    Point previousPosition = { -1, -1 };
};

#endif // DESKTOPENGINE_H
