#include <iostream>
#include <string>
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>

int main(int argc, char *argv[])
{
    if (argc < 4 || argc > 5)
    {
        std::cout << "Usage: " << argv[0] << " <image-file> <x-scale> <y-scale> [threshold]" << std::endl;
        return -1;
    }
    int x,y,comp;
    int numChannels = 1;
    int threshold = 24;
    char const *filename = argv[1];
    int xScale = atoi(argv[2]);
    int yScale = atoi(argv[3]);
    if (argc == 5)
        threshold = atoi(argv[4]);

    int res = stbi_info(filename, &x, &y, &comp);
    std::cout << "Original res=" << res << " x=" << x << " y=" << y <<" comp=" << comp << std::endl;

    uint8_t* img = stbi_load(filename, &x, &y, &comp, 0);
    if (img == NULL)
        return -1;

    uint8_t *imgScaled = (uint8_t *)malloc(xScale * yScale * comp);
    if (imgScaled == NULL) {
        std::cerr << "malloc failed" << std::endl;
        stbi_image_free(img);
        return -1;
    }
    if (!stbir_resize_uint8(img, x, y, 0, imgScaled, xScale, yScale, 0, comp))
    {
        std::cerr << "Unable to rescale" << std::endl;
        stbi_image_free(imgScaled);
        stbi_image_free(img);
        return -1;
    }
    std::cout << "Scaled x=" << xScale << " y=" << yScale <<" comp=" << comp << std::endl;
    for (int i=0; i<xScale; i++)
    {
        for (int j=0; j<yScale; j++)
        {
            uint8_t v = *(imgScaled + (i*xScale) + j);
            if (v >= threshold)
                std::cout << "X";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
    stbi_image_free(img);
    stbi_image_free(imgScaled);
    return 0;
}
