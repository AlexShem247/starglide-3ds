#define STB_IMAGE_IMPLEMENTATION
#include <algorithm> 
#include <cmath>
#include <iostream>

#include "n3DSEngine.h"
#include "colors.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 240

#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 240

#define CONSOLE_ENABLED false

using namespace std;

string getFilenameWithoutExtension(const string& filepath) {
    // Find the last occurrence of '/'
    size_t lastSlash = filepath.find_last_of("/\\");
    // Find the last occurrence of '.'
    size_t lastDot = filepath.find_last_of('.');

    // Extract the filename with extension
    string filename = (lastSlash == string::npos) ? filepath : filepath.substr(lastSlash + 1);
    
    // Extract the filename without extension
    if (lastDot != string::npos && lastDot > lastSlash) {
        return filename.substr(0, lastDot - lastSlash - 1);
    } else {
        return filename;  // No extension found, return the whole filename
    }
}


N3DSEngine::N3DSEngine(const char* title) : GameEngine(title) {
    romfsInit();
    cfguInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Create screen targets
    if (CONSOLE_ENABLED)
        consoleInit(GFX_BOTTOM, NULL);
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Prepare timer
    prevTime = svcGetSystemTick();
    ticksPerSecond = SYSCLOCK_ARM11;

    g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
}

N3DSEngine::~N3DSEngine() {
    // Cleanup resources if necessary
    // Deinitialise graphics
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

bool N3DSEngine::gameIsRunning() {
    return !gameIsTerminated && aptMainLoop();
}

void N3DSEngine::terminateGame() {
    gameIsTerminated = true;
}

void N3DSEngine::startDrawing() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
    C2D_SceneBegin(top);
}

void N3DSEngine::endDrawing() {
    C3D_FrameEnd(0);
    C2D_TextBufClear(g_staticBuf);
}

void N3DSEngine::clearBackground(RGB_Color color) {
    if (drawingBottom)
        C2D_TargetClear(bottom, C2D_Color32(color.r, color.g, color.b, color.a));
    else
        C2D_TargetClear(top, C2D_Color32(color.r, color.g, color.b, color.a));
}

void N3DSEngine::drawRect(Point p, double width, double height, RGB_Color fill) {
    u32 colorObj = C2D_Color32(fill.r, fill.g, fill.b, fill.a);
    C2D_DrawRectangle((int) p.x, (int) p.y, 0, (int) width, (int) height, colorObj, colorObj, colorObj, colorObj);
}

void N3DSEngine::drawLine(Point start, Point end, RGB_Color color) {
    u32 colorObj = C2D_Color32(color.r, color.g, color.b, color.a);
    C2D_DrawLine((int) start.x, (int) start.y, colorObj, (int) end.x, (int) end.y, colorObj, 1.0f, 1.0f);
}

void N3DSEngine::drawTriangle(Point p1, Point p2, Point p3, RGB_Color fill) {
    u32 colorObj = C2D_Color32(fill.r, fill.g, fill.b, fill.a);
    C2D_DrawTriangle((float) p1.x, (float) p1.y, colorObj,
                    (float) p2.x, (float) p2.y, colorObj,
                    (float) p3.x, (float) p3.y, colorObj, 1.0f);
}

void N3DSEngine::drawQuad(Point p1, Point p2, Point p3, Point p4, RGB_Color fill) {
    u32 colorObj = C2D_Color32(fill.r, fill.g, fill.b, fill.a);
    C2D_DrawTriangle((float) p1.x, (float) p1.y, colorObj,
                    (float) p2.x, (float) p2.y, colorObj,
                    (float) p3.x, (float) p3.y, colorObj, 0.9f);

    C2D_DrawTriangle((float) p1.x, (float) p1.y, colorObj,
                    (float) p3.x, (float) p3.y, colorObj,
                    (float) p4.x, (float) p4.y, colorObj, 0.9f);
}

void N3DSEngine::drawPoint(Point p, RGB_Color color) {
    drawRect(p, 1, 1, color);
}

void N3DSEngine::freeResources() {
    // Clear images
    for (Image img : images) {
        C2D_SpriteSheetFree(img.sheet); 
    }
    images.clear();
}

const vector<Key> N3DSEngine::calcHeldKeys() {
    vector<Key> keyVector;
    u32 kHeld = hidKeysHeld();
    if (kHeld & KEY_UP)
        keyVector.push_back(UP_KEY);
    if (kHeld & KEY_DOWN)
        keyVector.push_back(DOWN_KEY);
    if (kHeld & KEY_LEFT)
        keyVector.push_back(LEFT_KEY);
    if (kHeld & KEY_RIGHT)
        keyVector.push_back(RIGHT_KEY);
    if (kHeld & KEY_START)
        keyVector.push_back(START_KEY);
    if (kHeld & KEY_SELECT)
        keyVector.push_back(SELECT_KEY);
    if (kHeld & KEY_A)
        keyVector.push_back(PRIMARY_KEY);
    return keyVector;
}

// Function to calculate the difference between two sets of keys
vector<Key> minusSets(const vector<Key>& a, const vector<Key>& b) {
    vector<Key> result;
    for (const Key& key : a) {
        if (find(b.begin(), b.end(), key) == b.end()) {
            result.push_back(key);
        }
    }
    return result;
}

void N3DSEngine::scanInput() {
    hidScanInput();
    vector<Key> prevHeldKeys = heldKeys;
    heldKeys = calcHeldKeys();
    releasedKeys = minusSets(prevHeldKeys, heldKeys);
}

const vector<Key> N3DSEngine::getReleasedKeys() {
    return releasedKeys;
}

const vector<Key> N3DSEngine::getHeldKeys() {
    return heldKeys;
}

// Returns the (x, y) coordinates as a percentage of touchscreen
Point N3DSEngine::getTouchHeldPosition() {
    // Check if the screen is pressed (simulating a touch)
    touchPosition touch;
    hidTouchRead(&touch);

    // Use hidKeysHeld to check if the touchscreen is being touched
    u32 kHeld = hidKeysHeld();

    if (kHeld & KEY_TOUCH) {
        // If the screen is touched, calculate the touch position as a percentage
        Point pos;
        pos.x = (double) (touch.px) / TOUCH_WIDTH;
        pos.y = (double) (touch.py) / TOUCH_HEIGHT;

        return pos;
    }

    // If the screen is not pressed, return {-1, -1} to simulate a null touch
    return { -1, -1 };
}

// Returns the (x, y) coordinates as a percentage of tap
Point N3DSEngine::getTouchReleasedPosition() {
    // Read the current touch position
        touchPosition touch;
        hidTouchRead(&touch);

        // Check the current state of the touchscreen
        u32 kHeld = hidKeysHeld();

        bool isTouching = kHeld & KEY_TOUCH;

        // If the screen was touched before but is now released, return the position
        if (wasTouching && !isTouching) {
            Point pos;
            pos.x = static_cast<double>(touch.px) / TOUCH_WIDTH;
            pos.y = static_cast<double>(touch.py) / TOUCH_HEIGHT;

            wasTouching = false; // Update the previous state
            return pos;
        }

        // Update the previous state
        wasTouching = isTouching;

        // If no release was detected, return {-1, -1} to simulate a null touch
        return { -1, -1 };
}

// Returns the change in (x, y) since last frame in screenwidth/height percent
Point N3DSEngine::getTouchDragged() {
    // Get the current touch position
    Point currentPosition = getTouchHeldPosition();

    // Calculate the change in position as percentages
    double deltaX = currentPosition.x - previousPosition.x;
    double deltaY = currentPosition.y - previousPosition.y;

    // If the current position is {-1, -1}, return {0, 0} to indicate no movement
    if (currentPosition.x == -1 || previousPosition.x == -1) {
        deltaX = deltaY = 0;
    }

    // Return the delta as a Point
    previousPosition = currentPosition;
    return { deltaX, deltaY };
}

// Returns the time in seconds since the last frame
double N3DSEngine::getDeltaTime() {
    u64 currentTime = svcGetSystemTick();
    double deltaTime = (currentTime - prevTime) / ticksPerSecond;
    prevTime = currentTime;
    return deltaTime;
}

int N3DSEngine::getScreenWidth() {
    return WINDOW_WIDTH;
}
int N3DSEngine::getScreenHeight() {
    return WINDOW_HEIGHT;
}

int N3DSEngine::loadImage(const string& filename) {
    string imageName = getFilenameWithoutExtension(filename);
    C2D_SpriteSheet sheet = C2D_SpriteSheetLoad(("romfs:/gfx/" + imageName + ".t3x").c_str());	
	C2D_Image face = C2D_SpriteSheetGetImage(sheet, 0);

    Image img;
    img.sheet = sheet;
    img.face = face;
    images.push_back(img);
    return (int) images.size() - 1;
}

void N3DSEngine::drawImage(int id, Point p, double width, double height) {
    Image img = images[id];
    
    C2D_DrawImageAt(img.face, (float) p.x, (float) p.y, (float) (id % 2));
}

int N3DSEngine::loadFont(const string& filename) {
    string fontName = getFilenameWithoutExtension(filename);
    fonts[noFonts] = C2D_FontLoad(("romfs:/gfx/" + fontName + ".bcfnt").c_str());

    return noFonts++;
}

void N3DSEngine::drawText(int id, const string& text, Point p, bool center,
        double fontSize, double spacing, RGB_Color color) {
    float size = (float) fontSize / 20.0f;

    C2D_TextFontParse(&g_staticText[id], fonts[id], g_staticBuf, text.c_str());
    C2D_TextOptimize(&g_staticText[id]);

    u32 flags = C2D_WithColor;
    if (center)
        flags |= C2D_AlignCenter;
    
    C2D_DrawText(&g_staticText[id], flags, (float) p.x, (float) p.y - (fontSize / 1.25),
        0.0f, size, size, C2D_Color32(color.r, color.g, color.b, color.a));
}

void N3DSEngine::startDrawingLowerScreen() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_SceneBegin(bottom);
    drawingBottom = true;
}

void N3DSEngine::endDrawingLowerScreen() {
    C3D_FrameEnd(0);
    C2D_TextBufClear(g_staticBuf);
    drawingBottom = false;
}