#ifndef MATCH_H
#define MATCH_H

#include <vector>

struct Match {
    float x1, y1, x2, y2;
};

bool loadMatch(const char* nameFile, std::vector<Match>& match);
bool saveMatch(const char* nameFile, const std::vector<Match>& match);

#endif
