

#ifndef WALLPAPERPARSER_H
#define WALLPAPERPARSER_H

#include <string>
#include <vector>
#include "WallpaperFrame.h"
#include "DynamicWallpaper.h"

class WallpaperParser
{
public:
    DynamicWallpaper parse(std::string xmlPath);
};

#endif
