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

Vec3f get_barycentric_coords(Vec2i* points, Vec2i P)
{
    Vec3f u = Vec3f(points[2][0] - points[0][0], points[1][0] - points[0][0], points[0][0] - P[0]) ^ Vec3f(points[2][1] - points[0][1], points[1][1] - points[0][1], points[0][1] - P[1]);

    if (std::abs(u.z) < 1) return Vec3f(-1.0f, -1.0f, -1.0f); // If the triangle is degenerate, return something with negative coordinates.
    
    return Vec3f(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void draw_triangle(Vec2i* points, Image& image, const Color& color)
{
    Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i   clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        bboxmin.x = std::max(0, std::min(bboxmin.x, points[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, points[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, points[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, points[i].y));
    }

    Vec2i P;

    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f bc_screen = get_barycentric_coords(points, P);

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

            image.set(P.x, P.y, color);
        }
    }
}

int main(int argc, char** argv)
{
    Image image(IMAGE_WIDTH, IMAGE_HEIGHT, Color::Format::RGB);
    Model model("resources/models/human_head.obj");

    Vec3f light_direction(0.0f, 0.0f, -1.0f);

    for (int i = 0; i < model.get_nof_faces(); i++)
    {
        std::vector<int> face = model.get_face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];

        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model.get_vertex(face[j]);

            screen_coords[j] = Vec2i((int)((v.x + 1.0f) * IMAGE_WIDTH / 2.0f), (int)((v.y + 1.0f) * IMAGE_HEIGHT / 2.0f));
            world_coords[j] = v;
        }

        Vec3f face_normal = ((world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])).normalize();
        float intensity = face_normal * light_direction;

        if (intensity > 0.0f)
        {
            draw_triangle(screen_coords, image, Color(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically();
    image.write_on_disk("outputs/framebuffer_2.jpg", Image::FileFormat::JPG);

    return 0;
}