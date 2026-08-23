#ifndef CINNAMONBACKEND_H
#define CINNAMONBACKEND_H

#include "WallpaperBackend.h"

class CinnamonBackend : public WallpaperBackend
{
public:
    bool setWallpaper(const WallpaperFrame &frame, std::string &error) override;
    std::string getBackendName() const override;
    bool isAvailable(std::string &error) const override;
};

#endif
