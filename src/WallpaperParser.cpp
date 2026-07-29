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

        if (duration == nullptr)
        {
            std::cerr << "Missing <duration> element.\n";
            return frames;
        }

        const char* durationText = duration->GetText();

        if (durationText == nullptr)
        {
            std::cerr << "Duration element is empty.\n";
            return frames;
        }

        double durationValue =
            std::stod(durationText);

        tinyxml2::XMLElement* file =
            wallpaper->FirstChildElement("file");

        if (file == nullptr)
        {
            std::cerr << "Missing <file> element.\n";
            return frames;
        }

        const char* fileText = file->GetText();

        if (fileText == nullptr)
        {
            std::cerr << "File element is empty.\n";
            return frames;
        }

        std::string imagePath = fileText;

        WallpaperFrame frame(imagePath, durationValue);

        frames.push_back(frame);

        wallpaper = wallpaper->NextSiblingElement("static");
    }

    return frames;
}
