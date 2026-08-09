#include "WallpaperLibrary.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

WallpaperLibrary::WallpaperLibrary()
    : configDirectory(fs::path(std::getenv("HOME") ? std::getenv("HOME") : "") /
                      ".config/chronowall"),
      libraryFile(configDirectory / "wallpaper-library"),
      currentFile(configDirectory / "current-wallpaper"),
      configXml(configDirectory / "wallpaper.xml")
{
}

bool WallpaperLibrary::isConfigured() const
{
    return fs::is_directory(libraryPath());
}

std::filesystem::path WallpaperLibrary::libraryPath() const
{
    std::ifstream file(libraryFile);
    std::string value;
    if (!file || !std::getline(file, value))
        return {};

    value = trim(value);
    if (value.empty())
        return {};

    return fs::path(value);
}

std::string WallpaperLibrary::trim(const std::string &value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return {};

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string WallpaperLibrary::lower(const std::string &value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return result;
}

std::vector<std::string> WallpaperLibrary::list() const
{
    std::vector<std::string> wallpapers;
    const fs::path root = libraryPath();

    if (!fs::is_directory(root))
        return wallpapers;

    for (const auto &entry : fs::directory_iterator(root))
    {
        if (!entry.is_directory())
            continue;

        const std::string name = entry.path().filename().string();
        const fs::path xmlPath = entry.path() / (name + ".xml");

        if (fs::is_regular_file(xmlPath))
            wallpapers.push_back(name);
    }

    std::sort(wallpapers.begin(), wallpapers.end(),
              [](const std::string &a, const std::string &b) {
                  return lower(a) < lower(b);
              });

    return wallpapers;
}

std::filesystem::path WallpaperLibrary::findWallpaperXml(
    const std::string &name) const
{
    const fs::path root = libraryPath();
    if (!fs::is_directory(root))
        return {};

    for (const auto &entry : fs::directory_iterator(root))
    {
        if (!entry.is_directory())
            continue;

        const std::string candidate = entry.path().filename().string();
        if (lower(candidate) != lower(name))
            continue;

        const fs::path xmlPath = entry.path() / (candidate + ".xml");
        if (fs::is_regular_file(xmlPath))
            return xmlPath;
    }

    return {};
}

bool WallpaperLibrary::setWallpaper(const std::string &name,
                                     std::string &error) const
{
    if (!isConfigured())
    {
        error = "Wallpaper library is not configured. Reinstall a wallpaper with install.sh first.";
        return false;
    }

    const fs::path xmlPath = findWallpaperXml(name);
    if (xmlPath.empty())
    {
        error = "Wallpaper not found: " + name;
        return false;
    }

    const fs::path sourceDirectory = xmlPath.parent_path();
    fs::create_directories(configDirectory);

    const std::string oldName = current();

    try
    {
        const std::string newName = sourceDirectory.filename().string();
        const fs::path newPackageDirectory = configDirectory / newName;

        // Replace the selected package as a unit so stale assets from a
        // previous version of the wallpaper cannot remain behind.
        std::error_code cleanupError;
        fs::remove_all(newPackageDirectory, cleanupError);

        for (const auto &entry : fs::directory_iterator(sourceDirectory))
        {
            if (entry.path() == xmlPath)
                continue;

            const fs::path destination = configDirectory / entry.path().filename();
            if (entry.is_directory())
            {
                fs::copy(entry.path(), destination,
                         fs::copy_options::recursive |
                             fs::copy_options::overwrite_existing);
            }
            else
            {
                fs::copy_file(entry.path(), destination,
                              fs::copy_options::overwrite_existing);
            }
        }

        fs::copy_file(xmlPath, configXml,
                      fs::copy_options::overwrite_existing);

        {
            std::ofstream currentOutput(currentFile, std::ios::trunc);
            if (!currentOutput)
            {
                error = "Could not write current wallpaper state.";
                return false;
            }
            currentOutput << newName << '\n';
        }

        if (!oldName.empty() && lower(oldName) != lower(newName))
        {
            std::error_code ec;
            fs::remove_all(configDirectory / oldName, ec);
        }
    }
    catch (const fs::filesystem_error &exception)
    {
        error = exception.what();
        return false;
    }

    return true;
}

std::string WallpaperLibrary::current() const
{
    std::ifstream file(currentFile);
    std::string value;
    if (!file || !std::getline(file, value))
        return {};

    return trim(value);
}
