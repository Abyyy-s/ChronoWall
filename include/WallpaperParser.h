

#ifndef WALLPAPERPARSER_H
#define WALLPAPERPARSER_H

#include <string>
#include <vector>
#include "WallpaperFrame.h"

class WallpaperParser
{
public:
    std::vector<WallpaperFrame> parse(std::string xmlPath);
};

#endif
