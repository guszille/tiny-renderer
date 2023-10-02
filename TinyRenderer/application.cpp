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

Vec3f get_barycentric_coords(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
    Vec3f s[2] = { {C[0] - A[0], B[0] - A[0], A[0] - P[0]}, {C[1] - A[1], B[1] - A[1], A[1] - P[1]} };
    Vec3f u = s[0]^s[1];

    if (std::abs(u[2]) > 1e-2)
    {
        return Vec3f(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }

    return Vec3f(-1.0f, -1.0f, -1.0f);
}

void draw_triangle(Vec3f* points, float* zbuffer, Image& image, const Color& color)
{
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f   clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(    0.0f, std::min(bboxmin[j], points[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], points[i][j]));
        }
    }

    Vec3f P;

    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            Vec3f bccoords = get_barycentric_coords(points[0], points[1], points[2], P);

            if (bccoords.x < 0 || bccoords.y < 0 || bccoords.z < 0) continue;

            P.z = 0;

            for (int i = 0; i < 3; i++) P.z += points[i][2] * bccoords[i];

            if (zbuffer[int(P.x + P.y * IMAGE_WIDTH)] < P.z)
            {
                zbuffer[int(P.x + P.y * IMAGE_WIDTH)] = P.z;

                image.set(P.x, P.y, color);
            }
        }
    }
}

Vec3f world_to_screen(Vec3f v)
{
    return Vec3f(int((v.x + 1.0f) * IMAGE_WIDTH / 2.0f + 0.5f), int((v.y + 1.0f) * IMAGE_HEIGHT / 2.0f + 0.5f), v.z);
}

int main(int argc, char** argv)
{
    Image image(IMAGE_WIDTH, IMAGE_HEIGHT, Color::Format::RGB);
    Model model("resources/models/human_head.obj");

    Vec3f light_direction(0.0f, 0.0f, -1.0f);

    float* zbuffer = new float[IMAGE_WIDTH * IMAGE_HEIGHT];
    for (int i = IMAGE_WIDTH * IMAGE_HEIGHT; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    for (int i = 0; i < model.get_nof_faces(); i++)
    {
        std::vector<int> face = model.get_face(i);
        Vec3f wcoords[3]; // World coordinates.
        Vec3f scoords[3]; // Screen coordinates.

        for (int j = 0; j < 3; j++)
        {
            wcoords[j] = model.get_vertex(face[j]);
            scoords[j] = world_to_screen(wcoords[j]);
        }

        Vec3f face_normal = ((wcoords[2] - wcoords[0]) ^ (wcoords[1] - wcoords[0])).normalize();
        float intensity = face_normal * light_direction;

        if (intensity > 0.0f)
        {
            draw_triangle(scoords, zbuffer, image, Color(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically();
    image.write_on_disk("outputs/framebuffer_3.jpg", Image::FileFormat::JPG);

    return 0;
}