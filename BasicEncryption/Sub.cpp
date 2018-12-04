#include <secblock.h>
#include <aes.h>
#include <osrng.h>
#include <modes.h>

#include <iostream>
#include <string>
#include <memory>
#include <cmath>

#include "bitmap_image.hpp"
#include "Source\GlobalConfig.h"

using namespace CryptoPP;
typedef unsigned char byte;

#define AES_KEYLENGTH 32

#pragma pack(1)
class FlattenedRGBImage
{
public:
	FlattenedRGBImage(size_t width, size_t height) : _width(width), _height(height)
	{
		_red = std::unique_ptr<byte[]>(new byte[_width * _height]);
		_blue = std::unique_ptr<byte[]>(new byte[_width * _height]);
		_green = std::unique_ptr<byte[]>(new byte[_width * _height]);
	}

	byte* red()
	{
		return _red.get();
	}
	byte* blue()
	{
		return _blue.get();
	}
	byte* green()
	{
		return _green.get();
	}

private:
	size_t _width;
	size_t _height;
	std::unique_ptr<byte[]> _red;
	std::unique_ptr<byte[]> _blue;
	std::unique_ptr<byte[]> _green;
};

int quantise(int in, int levels)
{
	auto step = (int)256 / levels;
	auto n = (int)in / step;
	return n * levels;
}

int main()
{

	
	std::string path(DefaultTargetFile);

	// load bitmap image
	bitmap_image in_image(path);

	if (!in_image)
	{
		std::cerr << "Error - Failed to open file " << path << std::endl;
		return 1;
	}

	// flatten for encryption
	const auto height = in_image.height();
	const auto width = in_image.width();
	FlattenedRGBImage plain(width, height);
	auto ptr_red = plain.red();
	auto ptr_blue = plain.blue();
	auto ptr_green = plain.green();

	for (auto y = 0U; y < height; ++y)
	{
		for (auto x = 0U; x < width; ++x)
		{
			rgb_t colour;
			in_image.get_pixel(x, y, colour);
			*(ptr_red++) = (byte)quantise((int)colour.red, 4);
			*(ptr_blue++) = (byte)quantise((int)colour.blue, 4);
			*(ptr_green++) = (byte)quantise((int)colour.green, 4);
		}
	}

	// generate random key
	SecByteBlock key = SecByteBlock(AES::DEFAULT_KEYLENGTH);

	AutoSeededRandomPool rng;
	rng.GenerateBlock(key, key.size());

	FlattenedRGBImage cipher(width, height);

	try
	{
		// Encrypt using ECB mode
		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption enc;

		std::cout << 11 << std::endl;

		enc.SetKey(key, key.size());

		std::cout << 22 << std::endl;

		enc.ProcessData(cipher.red(), plain.red(), (size_t)(height * width));
		enc.ProcessData(cipher.blue(), plain.blue(), (size_t)(height * width));
		enc.ProcessData(cipher.green(), plain.green(), (size_t)(height * width));

		std::cout << 33 << std::endl;

		// Write to bitmap image
		ptr_red = cipher.red();
		ptr_blue = cipher.blue();
		ptr_green = cipher.green();
		bitmap_image out_image(width, height);
		for (auto y = 0U; y < height; ++y)
		{
			for (auto x = 0U; x < width; ++x)
			{
				rgb_t colour;
				colour.red = (char)*(ptr_red++);
				colour.blue = (char)*(ptr_blue++);
				colour.green = (char)*(ptr_green++);
				out_image.set_pixel(x, y, colour);
			}
		}
		out_image.save_image("ecb_encrypted.bmp");
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}