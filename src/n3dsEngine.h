#ifndef N3DSENGINE_H
#define N3DSENGINE_H

#include <3ds.h>
#include <citro2d.h>
#include <cstdint>
#include <cstddef>
#include <vector>

#include "gameEngine.h"
#include "colors.h"
#include "shapes.h"

#define MAX_NUM_FONTS 32

using namespace std;

struct Image {
    C2D_SpriteSheet sheet;
	C2D_Image face;
};

class N3DSEngine : public GameEngine {
public:
    // Constructor
    N3DSEngine(const char* title);

    // Destructor
    virtual ~N3DSEngine();

    // Implement the pure virtual methods from GameEngine
    bool gameIsRunning();
    void startDrawing();
    void clearBackground(RGB_Color color);
    void drawRect(Point p, double width, double height, RGB_Color fill);
    void drawLine(Point start, Point end, RGB_Color color);
    void drawTriangle(Point p1, Point p2, Point p3, RGB_Color fill);
    void drawQuad(Point p1, Point p2, Point p3, Point p4, RGB_Color fill);
    void drawPoint(Point p, RGB_Color color);
    void endDrawing();
    void freeResources();
    void startDrawingLowerScreen();
    void endDrawingLowerScreen();
    void terminateGame();

    int loadImage(const string& filename);
    void drawImage(int id, Point p, double width, double height);

    int loadFont(const string& filename);
    void drawText(int id, const string& text, Point p, bool center, double fontSize,
        double spacing, RGB_Color color);

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
    C3D_RenderTarget* top;
    C3D_RenderTarget* bottom;
    u64 prevTime;
    double ticksPerSecond;
    vector<Image> images;

    C2D_TextBuf g_staticBuf;
    C2D_Text g_staticText[MAX_NUM_FONTS];
    C2D_Font fonts[MAX_NUM_FONTS];
    int noFonts = 0;

    const vector<Key> calcHeldKeys();
    vector<Key> heldKeys;
    vector<Key> releasedKeys;

    bool wasTouching = false;
    bool gameIsTerminated = false;
    bool drawingBottom = false;
    Point previousPosition = { -1, -1 };
};

#endif // N3DSENGINE_H
