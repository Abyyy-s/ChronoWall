#include "WallpaperParser.h"
#include <iostream>

int main()
{
    WallpaperParser parser;

    auto frames = parser.parse("../MagicLake.xml");

    for (const auto& frame : frames)
    {
        std::cout << "Image: "
                  << frame.getImagePath()
                  << std::endl;

        std::cout << "Duration: "
                  << frame.getDuration()
                  << std::endl;

        std::cout << "-----------------------"
                  << std::endl;
    }

    return 0;
}
