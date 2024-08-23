#include "keys.h"
#include <algorithm> 

bool contains(const std::vector<Key>& keys, Key key) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}
