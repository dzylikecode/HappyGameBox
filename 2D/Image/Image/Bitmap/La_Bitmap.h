#pragma once


typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

namespace la
{
	void ReadImage(const char* fileName, byte** pixels, int32* width, int32* height, int32* bytesPerPixel);
	void WriteImage(const char* fileName, byte* pixels, int32 width, int32 height, int32 bytesPerPixel);
}
