// Tiny Renderer.

#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "libs/image.h"

const int IMAGE_WIDTH  = 100;
const int IMAGE_HEIGHT = 100;

void draw_line(int x0, int y0, int x1, int y1, Image& image, const Color& color)
{
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);

        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int delta_error = std::abs(dy) * 2;
    int error = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color); // if transposed, de−transpose.
        }
        else
        {
            image.set(x, y, color);
        }

        error += delta_error;

        if (error > dx)
        {
            error -= dx * 2;
            y += y1 > y0 ? 1 : -1;
        }
    }
}

int main(int argc, char** argv)
{
    Image image(IMAGE_WIDTH, IMAGE_HEIGHT, Color::Format::RGB);

    draw_line(13, 20, 80, 40, image, WHITE);
    draw_line(20, 13, 40, 80, image, RED);
    draw_line(80, 40, 13, 20, image, RED);

    image.flip_vertically();
    image.write_on_disk("outputs/image.tga", Image::FileFormat::TGA);

    return 0;
}