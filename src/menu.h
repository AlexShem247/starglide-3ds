#ifndef MENU_H
#define MENU_H

#include <string>

#include "gameEngine.h"
#include "resources.h"

using namespace std;

// Displays game menu with a title and a button
int showMenu(GameEngine& gameEngine, GameResources& res, const string& titleText, const string& btnText,
              const string& btnScreenText, const string& message);

#endif // MENU_H
