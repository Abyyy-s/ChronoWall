#ifndef WALLPAPERBACKENDFACTORY_H
#define WALLPAPERBACKENDFACTORY_H

#include "WallpaperBackend.h"
#include <memory>
#include <string>

class WallpaperBackendFactory
{
public:
    static std::unique_ptr<WallpaperBackend> createBackend(std::string &error);
    static std::unique_ptr<WallpaperBackend> createBackendByName(const std::string &name, std::string &error);
};

#endif
