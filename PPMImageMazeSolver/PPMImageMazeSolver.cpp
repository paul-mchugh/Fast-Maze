// PPMImageMazeSolver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "PPMReadWrite.h"
#include "MazeImageProcessor.h"


int main(int argc, char *argv[])
{
	if (argc!=7)
	{
		std::cout << "You passed " << (argc - 1) << " arguments." << std::endl;
		std::exit(0);
	}
	ImageData data;

	int startX = std::atoi(argv[3]);
	int startY = std::atoi(argv[4]);
	int endX = std::atoi(argv[5]);
	int endY = std::atoi(argv[6]);

	try
	{
		PPMReadWrite ppmFile = PPMReadWrite(std::string(argv[1]));
		data = ppmFile.createImageData();
	}
	catch (const std::string& reason)
	{
		std::cout << "Error:" << reason << std::endl;
		std::exit(1);
	}

	MazeImageProcessor processor = MazeImageProcessor(data);

	PPMReadWrite output(processor.getImageWithPath(startX, startY, endX, endY));
	output.writeToFile(std::string(argv[2]));

    return 0;
}

