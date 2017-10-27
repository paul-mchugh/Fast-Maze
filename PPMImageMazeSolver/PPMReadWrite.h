#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct RGBPixel
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	bool badReq = false;
};

struct ImageData
{
	std::vector<unsigned char> red;
	std::vector<unsigned char> green;
	std::vector<unsigned char> blue;

	int width = -1;
	int height = -1;
	bool badData = false;
};

class PPMReadWrite
{
	//This class can load binary PPM image files (P6).  If 16-bit color is being used it is automatically downsampled to 8-bit color.
	//All sub 8-bit color is upsampled to 8-bit color.  Files are stored in sRGB.

	//class variables
	std::vector<unsigned char> red;
	std::vector<unsigned char> green;
	std::vector<unsigned char> blue;

	int width = -1;
	int height = -1;
	int fileMaxValue = -1;
	bool isTwoByte = false;

public:
	PPMReadWrite();
	PPMReadWrite(int width, int height);
	PPMReadWrite(ImageData data);
	PPMReadWrite(std::string fileName);
	ImageData createImageData();
	bool writeToFile(std::string fileName);
	RGBPixel getRGB(int x, int y);
	void setRGB(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
	void setRGB(int x, int y, RGBPixel pixelValue);
private:
	void skipToASCIIInteger(std::ifstream & stream);
};

