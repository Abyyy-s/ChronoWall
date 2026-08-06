#include "WallpaperParser.h"

#include <iostream>
#include <string>
#include <tinyxml2.h>
#include <vector>

DynamicWallpaper WallpaperParser::parse(std::string xmlPath)
{
    DynamicWallpaper wallpaper;
    std::vector<WallpaperFrame> frames;

    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(xmlPath.c_str()) != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Failed to load XML file: "
                  << xmlPath << '\n';
        return wallpaper;
    }

    tinyxml2::XMLElement *root =
        doc.FirstChildElement("background");

    if (root == nullptr)
    {
        std::cerr << "No <background> element found.\n";
        return wallpaper;
    }

    // -----------------------------
    // Parse <starttime>
    // -----------------------------

    tinyxml2::XMLElement *startTime =
        root->FirstChildElement("starttime");

    if (startTime != nullptr)
    {
        tinyxml2::XMLElement *hour =
            startTime->FirstChildElement("hour");

        tinyxml2::XMLElement *minute =
            startTime->FirstChildElement("minute");

        tinyxml2::XMLElement *second =
            startTime->FirstChildElement("second");

        if (hour && minute && second)
        {
            int startTimeSeconds =
                std::stoi(hour->GetText()) * 3600 +
                std::stoi(minute->GetText()) * 60 +
                std::stoi(second->GetText());

            wallpaper.setStartTime(startTimeSeconds);
        }
    }

    // -----------------------------
    // Parse <static> elements
    // -----------------------------

    tinyxml2::XMLElement *wallpaperElement =
        root->FirstChildElement("static");

    while (wallpaperElement != nullptr)
    {
        tinyxml2::XMLElement *duration =
            wallpaperElement->FirstChildElement("duration");

        if (duration == nullptr)
        {
            std::cerr << "Missing <duration> element.\n";
            return wallpaper;
        }

        tinyxml2::XMLElement *file =
            wallpaperElement->FirstChildElement("file");

        if (file == nullptr)
        {
            std::cerr << "Missing <file> element.\n";
            return wallpaper;
        }

        if (duration->GetText() == nullptr)
        {
            std::cerr << "Empty <duration>.\n";
            return wallpaper;
        }

        if (file->GetText() == nullptr)
        {
            std::cerr << "Empty <file>.\n";
            return wallpaper;
        }

        double durationValue =
            std::stod(duration->GetText());

        std::string imagePath =
            file->GetText();

        WallpaperFrame frame(
            imagePath,
            durationValue);

        frames.push_back(frame);

        wallpaperElement =
            wallpaperElement->NextSiblingElement("static");
    }

    wallpaper.setFrames(frames);

    return wallpaper;
}
