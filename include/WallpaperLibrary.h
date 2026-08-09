#ifndef WALLPAPERLIBRARY_H
#define WALLPAPERLIBRARY_H

#include <filesystem>
#include <string>
#include <vector>

class WallpaperLibrary
{
public:
    WallpaperLibrary();

    bool isConfigured() const;
    std::vector<std::string> list() const;
    bool setWallpaper(const std::string &name, std::string &error) const;
    std::string current() const;

private:
    std::filesystem::path configDirectory;
    std::filesystem::path libraryFile;
    std::filesystem::path currentFile;
    std::filesystem::path configXml;

    std::filesystem::path libraryPath() const;
    std::filesystem::path findWallpaperXml(const std::string &name) const;
    static std::string lower(const std::string &value);
    static std::string trim(const std::string &value);
};

#endif
