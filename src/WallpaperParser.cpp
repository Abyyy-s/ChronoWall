#include "WallpaperParser.h"
#include <tinyxml2.h>
#include <iostream>
#include <string>
#include <vector>

std::vector<WallpaperFrame> WallpaperParser::parse(std::string xmlPath)
{
    std::vector<WallpaperFrame> frames;

    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(xmlPath.c_str()) != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Failed to load XML file: " << xmlPath << '\n';
        return frames;
    }

    tinyxml2::XMLElement* root =
        doc.FirstChildElement("background");

    if (root == nullptr)
    {
        std::cerr << "No <background> element found.\n";
        return frames;
    }

    tinyxml2::XMLElement* wallpaper =
        root->FirstChildElement("static");

    while (wallpaper != nullptr)
    {
        tinyxml2::XMLElement* duration =
            wallpaper->FirstChildElement("duration");

        double durationValue =
            std::stod(duration->GetText());

        tinyxml2::XMLElement* file =
            wallpaper->FirstChildElement("file");

        std::string imagePath =
            file->GetText();

        WallpaperFrame frame(imagePath, durationValue);

        frames.push_back(frame);

        wallpaper = wallpaper->NextSiblingElement("static");
    }

    return frames;
}
