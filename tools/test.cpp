#include <iostream>
#include <stb/stb_image.h>

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " <image-file> [threshold]" << std::endl;
        return -1;
    }
    int x,y,comp;
    int threshold = 24;
    char const *filename = argv[1];
    if (argc == 3)
        threshold = atoi(argv[2]);

    int res = stbi_info(filename, &x, &y, &comp);
    std::cout << "res=" << res << " x=" << x << " y=" << y <<" comp=" << comp << std::endl;

    uint8_t* img = stbi_load(filename, &x, &y, &comp, 0);
    if (img == NULL)
        return -1;

    for (int i=0; i<x; i++)
    {
        for (int j=0; j<y; j++)
        {
            uint8_t v = *(img + (i*x) + j);
            if (v >= threshold)
                std::cout << "X";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
    stbi_image_free(img);
    return 0;
}
