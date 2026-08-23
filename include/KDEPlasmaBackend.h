#ifndef KDEPLASMABACKEND_H
#define KDEPLASMABACKEND_H

#include "WallpaperBackend.h"

class KDEPlasmaBackend : public WallpaperBackend
{
public:
    bool setWallpaper(const WallpaperFrame &frame, std::string &error) override;
    std::string getBackendName() const override;
    bool isAvailable(std::string &error) const override;
};

#endif
