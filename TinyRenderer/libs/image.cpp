#include "image.h"

Image::Image(int width, int height, int channels)
	: width(width), height(height), channels(channels), data(new unsigned char[width * height * channels])
{
	memset(data, 0, width * height * channels);
}

void Image::set(int x, int y, const Color& color)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= height) return;

	int index = (x + y * width) * channels;

	data[index++] = color.r;
	data[index++] = color.g;
	data[index++] = color.b;

	if (channels == Color::Format::RGBA)
	{
		data[index++] = color.a;
	}
}

void Image::flip_vertically()
{
	if (!data) return;

	unsigned long  bytes_per_line = width * channels;
	unsigned char* line_buffer = new unsigned char[bytes_per_line];
	int half = height >> 1;

	for (int j = 0; j < half; j++)
	{
		unsigned long offset_1 = j * bytes_per_line;
		unsigned long offset_2 = (height - 1 - j) * bytes_per_line;

		memmove((void*)line_buffer, (void*)(data + offset_1), bytes_per_line);
		memmove((void*)(data + offset_1), (void*)(data + offset_2), bytes_per_line);
		memmove((void*)(data + offset_2), (void*)line_buffer, bytes_per_line);
	}

	delete[] line_buffer;
}

void Image::write_on_disk(const char* filename, FileFormat format)
{
	switch (format)
	{
	case FileFormat::TGA:
		stbi_write_tga(filename, width, height, channels, data);
		break;

	case FileFormat::PNG:
		stbi_write_png(filename, width, height, channels, data, width * channels);
		break;

	case FileFormat::JPG:
		stbi_write_jpg(filename, width, height, channels, data, 100);
		break;

	default:
		std::cout << "Unsupported image file format!" << std::endl;
		break;
	}
}

int Image::get_width()
{
	return width;
}

int Image::get_height()
{
	return height;
}
