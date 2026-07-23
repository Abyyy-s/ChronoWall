#include <iostream>
#include "WallpaperFrame.h"

int main()
{
    WallpaperFrame frame("Carvan-1.png", 42300);

    std::cout << frame.getImagePath() << std::endl;
    std::cout << frame.getDuration() << std::endl;

    return 0;
}
