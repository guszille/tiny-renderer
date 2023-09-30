// Tiny Renderer.

#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "libs/image.h"
#include "libs/model.h"

const int IMAGE_WIDTH  = 800;
const int IMAGE_HEIGHT = 800;

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
    Model model("resources/models/human_head.obj");

    for (int i = 0; i < model.get_nof_faces(); i++)
    {
        std::vector<int> face = model.get_face(i);

        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model.get_vertex(face[j]);
            Vec3f v1 = model.get_vertex(face[(j + 1) % 3]);

            int x0 = (int)((v0.x + 1.0f) * IMAGE_WIDTH  / 2.0f);
            int y0 = (int)((v0.y + 1.0f) * IMAGE_HEIGHT / 2.0f);
            int x1 = (int)((v1.x + 1.0f) * IMAGE_WIDTH  / 2.0f);
            int y1 = (int)((v1.y + 1.0f) * IMAGE_HEIGHT / 2.0f);

            draw_line(x0, y0, x1, y1, image, WHITE);
        }
    }

    image.flip_vertically();
    image.write_on_disk("outputs/wireframe.tga", Image::FileFormat::TGA);

    return 0;
}