#include <algorithm> 
#include <cmath>    
#include <iostream>

#include "colors.h"
#include "keys.h"
#include "desktopEngine.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 400

using namespace std;

// Helper function to calculate the signed area of a triangle
double signedArea(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

// Function to determine if points are in clockwise order
bool isClockwise(Point p1, Point p2, Point p3, Point p4) {
    double area1 = signedArea(p1, p2, p3);
    double area2 = signedArea(p1, p3, p4);
    return (area1 + area2) < 0;
}

// Function to sort the points into the correct order
void sortPoints(Point& p1, Point& p2, Point& p3, Point& p4) {
    // Check for all possible point arrangements and reorder them
    if (!isClockwise(p1, p2, p3, p4)) {
        // If not in clockwise order, swap p3 and p4
        swap(p3, p4);
    }

    // After swapping, ensure p1, p2, p3, p4 form a convex quadrilateral
    if (!isClockwise(p1, p2, p3, p4)) {
        // If the order is still not correct, swap p1 and p2
        swap(p1, p2);
        // After swapping p1 and p2, p3 and p4 should be in the correct order
        // Recheck and swap if necessary
        if (!isClockwise(p1, p2, p3, p4)) {
            swap(p3, p4);
        }
    }
}


DesktopEngine::DesktopEngine(const char* title) : GameEngine(title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);  // Initialize window with title
    SetTargetFPS(60);  // Set frame rate to 60 FPS
}

DesktopEngine::~DesktopEngine() {
    // Cleanup resources if necessary
    CloseWindow();  // Close the window
}

bool DesktopEngine::gameIsRunning() {
    return !gameIsTerminated && !WindowShouldClose();  // Check if the window should close
}

void DesktopEngine::terminateGame() {
    gameIsTerminated = true;
}

void DesktopEngine::startDrawing() {
    BeginDrawing();  // Start drawing
}

void DesktopEngine::clearBackground(RGB_Color color) {
    if (drawing)
        ClearBackground({ color.r, color.g, color.b, color.a});  // Clear screen with specified color
}

void DesktopEngine::drawRect(Point p, double width, double height, RGB_Color fill) {
    if (drawing)
        DrawRectangle((int) p.x, (int) p.y, (int) width, (int) height, { fill.r, fill.g, fill.b, fill.a });
}

void DesktopEngine::drawLine(Point start, Point end, RGB_Color color) {
    if (drawing)
        DrawLine((int) start.x, (int) start.y, (int) end.x, (int) end.y, { color.r, color.g, color.b, color.a });
}

void DesktopEngine::drawTriangle(Point p1, Point p2, Point p3, RGB_Color fill) {
    // Check if the points are in counterclockwise order
    if (signedArea(p1, p2, p3) >= 0) {
        // Swap p2 and p3 to ensure counterclockwise order
        Point temp = p2;
        p2 = p3;
        p3 = temp;
    }

    if (drawing)
        DrawTriangle(
            { (float) p1.x, (float) p1.y },
            { (float) p2.x, (float) p2.y },
            { (float) p3.x, (float) p3.y },
            { fill.r, fill.g, fill.b, fill.a }
        );
}

void DesktopEngine::drawQuad(Point p1, Point p2, Point p3, Point p4, RGB_Color fill) {
    // Sort the points into the correct order
    sortPoints(p1, p2, p3, p4);

    // Draw the first triangle (p1, p2, p3)
    if (drawing)
        DrawTriangle(
            { (float) p1.x, (float) p1.y },
            { (float) p2.x, (float) p2.y },
            { (float) p3.x, (float) p3.y },
            { fill.r, fill.g, fill.b, fill.a }
        );

    // Draw the second triangle (p1, p3, p4)
    if (drawing)
        DrawTriangle(
            { (float) p1.x, (float) p1.y },
            { (float) p3.x, (float) p3.y },
            { (float) p4.x, (float) p4.y },
            { fill.r, fill.g, fill.b, fill.a }
        );
}

void DesktopEngine::endDrawing() {
    EndDrawing();  // End drawing
}

void DesktopEngine::freeResources() {
    // Clear textures
    for (Texture2D& texture : textures) {
        UnloadTexture(texture); 
    }
    textures.clear();
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

void DesktopEngine::scanInput() {
    vector<Key> prevHeldKeys = heldKeys;
    heldKeys = calcHeldKeys();
    releasedKeys = minusSets(prevHeldKeys, heldKeys);
}

const vector<Key> DesktopEngine::calcHeldKeys() {
    vector<Key> keyVector;
    if (IsKeyDown(KeyboardKey::KEY_W))
        keyVector.push_back(UP_KEY);
    if (IsKeyDown(KeyboardKey::KEY_S))
        keyVector.push_back(DOWN_KEY);
    if (IsKeyDown(KeyboardKey::KEY_A))
        keyVector.push_back(LEFT_KEY);
    if (IsKeyDown(KeyboardKey::KEY_D))
        keyVector.push_back(RIGHT_KEY);
    if (IsKeyDown(KeyboardKey::KEY_ENTER))
        keyVector.push_back(START_KEY);
    if (IsKeyDown(KeyboardKey::KEY_BACKSPACE))
        keyVector.push_back(SELECT_KEY);
    if (IsKeyDown(KeyboardKey::KEY_SPACE))
        keyVector.push_back(PRIMARY_KEY);
    return keyVector;
}

const vector<Key> DesktopEngine::getReleasedKeys() {
    return releasedKeys;
}

const vector<Key> DesktopEngine::getHeldKeys() {
    return heldKeys;
}

// Returns the (x, y) coordinates as a percentage of touchscreen
Point DesktopEngine::getTouchHeldPosition() {
    // Check if the mouse is pressed (simulating a touch)
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        // Get the mouse position
        Point pos;
        pos.x = GetMousePosition().x / getScreenWidth();
        pos.y = GetMousePosition().y / getScreenHeight();

        return pos;
    }

    // If the mouse is not pressed, return {-1, -1} to simulate a null touch
    return { -1, -1 };
}

// Returns the (x, y) coordinates as a percentage of tap
Point DesktopEngine::getTouchReleasedPosition() {
    // Check the current state of the mouse button
    bool isMousePressed = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    // If the mouse was pressed before but is now released, return the position
    if (wasMousePressed && !isMousePressed) {
        Point pos;
        pos.x = GetMousePosition().x / getScreenWidth();
        pos.y = GetMousePosition().y / getScreenHeight();

        wasMousePressed = false; // Update the previous state
        return pos;
    }

    // Update the previous state
    wasMousePressed = isMousePressed;

    // If no release was detected, return a null indicator, such as {-1, -1}
    return { -1, -1 };
}

// Returns the change in (x, y) since last frame in screenwidth/height percent
Point DesktopEngine::getTouchDragged() {
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
double DesktopEngine::getDeltaTime() {
    return GetFrameTime();
}

int DesktopEngine::getScreenWidth() {
    return GetScreenWidth();
}
int DesktopEngine::getScreenHeight() {
    return GetScreenHeight();
}

int DesktopEngine::loadImage(const string& filename) {
    Texture2D texture = LoadTexture(filename.c_str());

    // Check if the texture was successfully loaded
    if (texture.width == 0 && texture.height == 0) {
        // Texture loading failed, return -1
        return -1;
    }

    textures.push_back(texture);
    return (int) textures.size() - 1;
}

void DesktopEngine::drawImage(int id, Point p, double width, double height) {
    if (drawing) {
        Texture2D texture = textures[id];
        Rectangle sourceRect = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle destRect = { (float)p.x, (float)p.y, (float)width, (float)height };
        Vector2 origin = { 0.0f, 0.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, 0.0f, WHITE);
    }
}

int DesktopEngine::loadFont(const string& filename) {
    Font font = LoadFontEx(filename.c_str(), getScreenHeight(), nullptr, 0);
    fonts.push_back(font);
    return (int) fonts.size() - 1;
}

void DesktopEngine::drawText(int id, const string& text, Point p, bool center, double fontSize,
    double spacing, RGB_Color color) {
    if (drawing) {
        Font font = fonts[id];
        Vector2 textPosition;

        if (center) {
            // Measure the text size
            Vector2 textSize = MeasureTextEx(font, text.c_str(), (float)fontSize, (float)spacing);

            // Calculate the position to center the text
            textPosition = {
                (float)p.x - (textSize.x / 2),
                (float)p.y - (textSize.y / 2)
            };
        } else {
            textPosition = { (float) p.x, (float) p.y };
        }

        

        // Draw the text at the calculated position
        DrawTextEx(font, text.c_str(), textPosition, (float) fontSize, (float) spacing,
            { color.r, color.g, color.b, color.a });
    }
}

void DesktopEngine::startDrawingLowerScreen() {
    drawing = false;
}
void DesktopEngine::endDrawingLowerScreen() {
    drawing = true;
}