#ifndef KEYS_H
#define KEYS_H

#include <vector>

enum Key { START_KEY, SELECT_KEY, UP_KEY, DOWN_KEY, LEFT_KEY, RIGHT_KEY, PRIMARY_KEY };

// Function to check if a key is in a vector of keys
bool contains(const std::vector<Key>& keys, Key key);

#endif // KEYS_H
