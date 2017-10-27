#pragma once
#include "PPMReadWrite.h"
#include <vector>
#include <list>

class MazeImageProcessor
{
	//static variables
	static const unsigned char TOLERENCE = 64;

	//instance variables
	ImageData data;

public:
	MazeImageProcessor();
	MazeImageProcessor(ImageData iData);
	ImageData getImageWithPath(int startX, int startY, int endX, int endY);
	~MazeImageProcessor();
private:
	int calculateHeuristic(int pixelNo, int endX, int endY, int width);
};

