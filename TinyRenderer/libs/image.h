#pragma once

#include <iostream>

#include "vendor/stb/stb_image_write.h"

struct Color
{
	union
	{
		struct { unsigned char r, g, b, a; };
		unsigned char raw[4];
		unsigned int  value;
	};

	enum Format
	{
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};

	Color() : value(0) {}
	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) : r(red), g(green), b(blue), a(alpha) {}
};

class Image
{
public:
	Image(int width, int height, int channels);

	enum class FileFormat
	{
		TGA, PNG, JPG
	};

	void set(int x, int y, const Color& color);
	void flip_vertically();
	void write_on_disk(const char* filename, FileFormat format);

	int get_width();
	int get_height();

private:
	unsigned char* data;
	int width, height, channels;
};

const Color WHITE = Color(255, 255, 255, 255);
const Color BLACK = Color(  0,   0,   0, 255);
const Color RED   = Color(255,   0,   0, 255);
const Color GREEN = Color(  0, 255,   0, 255);
const Color BLUE  = Color(  0,   0, 255, 255);
