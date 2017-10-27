#include "stdafx.h"
#include "PPMReadWrite.h"
#include <limits.h>

PPMReadWrite::PPMReadWrite()
{
}

PPMReadWrite::PPMReadWrite(int width, int height)
{
	this->width = width;
	this->height = height;
	red.resize(width*height);
	green.resize(width*height);
	blue.resize(width*height);
}

PPMReadWrite::PPMReadWrite(ImageData data) :red(data.red), green(data.green), blue(data.blue), width(data.width), height(data.height)
{

}

PPMReadWrite::PPMReadWrite(std::string fileName)
{
	//open the named file
	std::ifstream filePPM;
	filePPM.open(fileName, std::ios::binary);

	if (filePPM.is_open())
	{
		//continue
		//copy the first three characters to a string to check the file starts with "P6<whitespace character>"
		char magicNumberReturn[3];
		filePPM.read(magicNumberReturn, 3);
		unsigned char * magicNumber = reinterpret_cast<unsigned char*>(magicNumberReturn);
		if (magicNumber[0]=='P'&&magicNumber[1]=='6'&&magicNumber[2]>=9&&magicNumber[2]<=13)
		{
			//continue
			//read width
			skipToASCIIInteger(filePPM);
			filePPM >> width;
			//read height
			skipToASCIIInteger(filePPM);
			filePPM >> height;
			//read the max pixel value
			skipToASCIIInteger(filePPM);
			filePPM >> fileMaxValue;
			//get rid of the seperator char
			filePPM.get();

			//now I need to check if the height, width and maxValue header items are reasonable.
			//acorrding to PPM P6 specifications the maxval (maximum color value) must be less than 65536 and greater than 0
			if (width>0&&height>0&&fileMaxValue>0&&fileMaxValue<65536)
			{
				//set isTwoByte to the appropriate value
				isTwoByte = fileMaxValue > 255;
				//get how many bytes remain in the file and check to see if it matches the number of bytes that should be stored in the raster
				std::streampos currentPosition = filePPM.tellg();
				filePPM.seekg(0, std::ios::end);
				std::streampos totalSize = filePPM.tellg();
				filePPM.seekg(currentPosition);
				
				//calculate the number of remaining bytes
				long remainingBytes = (long)(totalSize - currentPosition);
				if (remainingBytes==width*height*3*(isTwoByte?2:1))
				{
					red.resize(width*height);
					green.resize(width*height);
					blue.resize(width*height);

					if (!isTwoByte)
					{
						unsigned char pixel[3] = { 0,0,0 };
						for (int i = 0; i < width*height*3; i+=3)
						{
							filePPM.read((char *)pixel, 3);
							red[i / 3] = pixel[0] * 255 / fileMaxValue;
							green[i / 3] = pixel[1] * 255 / fileMaxValue;
							blue[i / 3] = pixel[2] * 255 / fileMaxValue;
						}
					}
					else
					{
						unsigned char pixel[6] = { 0,0,0,0,0,0 };
						for (int i = 0; i < width*height*6; i+=6)
						{
							//PPM P6 two byte storage stores the values with the MSB first (big endian).  If I read the LSB I would just have to truncate it.
							//Just read and rescale the MSB instead.
							filePPM.read((char *)pixel, 6);
							red[i / 6] = pixel[0] * 225 / fileMaxValue;
							green[i / 6] = pixel[2] * 255 / fileMaxValue;
							blue[i / 6] = pixel[4] * 255 / fileMaxValue;
						}
					}
					filePPM.close();
				}
				else
				{
					//There is a different number of bytes than expected
					//TODO:Close stream, throw an execption, and fail
					filePPM.close();
					throw std::string("There is a different number of bytes than expected");
				}
			}
			else
			{
				//The header is invalid
				filePPM.close();
				throw std::string("The header is invalid");
			}
		}
		else
		{
			//The magic numbe was wrong
			filePPM.close();
			throw std::string("The magic numbe was wrong");
		}
	}
	else
	{
		//The file couldn't be opened
		throw std::string("The file couldn't be opened");
	}

}

ImageData PPMReadWrite::createImageData()
{
	ImageData iData;
	iData.width = width;
	iData.height = height;
	iData.red = red;
	iData.green = green;
	iData.blue = blue;

	return iData;
}

bool PPMReadWrite::writeToFile(std::string fileName)
{
	std::ofstream outputFile;
	outputFile.open(fileName, std::ios::binary);

	if (outputFile.is_open())
	{
		//write the header
		outputFile << "P6\n";
		outputFile << std::to_string(width) << " ";
		outputFile << std::to_string(height) << "\n";
		outputFile << "255\n";
		
		char pixel[3] = {0,0,0};
		for (int i = 0; i < width*height; i++)
		{
			pixel[0] = red[i];
			pixel[1] = green[i];
			pixel[2] = blue[i];
			outputFile.write(pixel, 3);
		}
		return true;
	}
	return false;
}

RGBPixel PPMReadWrite::getRGB(int x, int y)
{
	if (x < width && y < height)
	{
		RGBPixel failValue = { 0,0,0,true };
		return failValue;
	}
	else
	{
		int index = y*width + x;
		RGBPixel pixelValue = { red[index],green[index],blue[index], false };
		return pixelValue;
	}
}

void PPMReadWrite::setRGB(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
{
	if (x < width && y < height)
	{
		int index = y*width + x;
		this->red[index] = red;
		this->green[index] = green;
		this->blue[index] = blue;
	}
}

void PPMReadWrite::setRGB(int x, int y, RGBPixel pixelValue)
{
	setRGB(x, y, pixelValue.red, pixelValue.green, pixelValue.blue);
}

void PPMReadWrite::skipToASCIIInteger(std::ifstream & stream)
{
	bool inComment = false;
	while (!(stream.peek()>=48&&stream.peek()<=57)||inComment)
	{
		int check = stream.get();
		if (!inComment)
		{
			if (check=='#')
			{
				inComment = true;
			}
		}
		else if (check==10)
		{
			inComment = false;
		}
	}
}
