#include "stdafx.h"
#include "MazeImageProcessor.h"


MazeImageProcessor::MazeImageProcessor()
{
}

MazeImageProcessor::MazeImageProcessor(ImageData iData)
{
	//copy the ImageData structure which stores the bmp
	data = iData;

	//greyscale the image
	for (int i = 0; i < data.width*data.height; i++)
	{
		short average = (data.red[i] + data.green[i] + data.blue[i])/3;
		data.red[i] = (unsigned char)average;
		data.green[i] = (unsigned char)average;
		data.blue[i] = (unsigned char)average;
	}

	//white out the pixels that are close to white
	for (int i = 0; i < data.width*data.height; i++)
	{
		if (data.red[i]>=255-TOLERENCE)
		{
			data.red[i] = 255;
			data.green[i] = 255;
			data.blue[i] = 255;
		}
	}
}

ImageData MazeImageProcessor::getImageWithPath(int startX, int startY, int endX, int endY)
{
	int startNode = startY*data.width + startX;
	int endNode = endY*data.width + endX;

	if ((startX==endX&&startY==endY)||data.red[startNode]!=255||data.red[endNode]!=255)
	{
		return data;
	}
	if (startX >= 0 && startX<data.width&&startY >= 0 && startY<data.height&&endX >= 0 && endX<data.width&&endY >= 0 && endY<data.height)
	{
		std::list<int> openList;
		std::vector<int> backNode(data.width*data.height,-1);
		std::vector<int> totalMovementCost(data.width*data.height, -1);
		//the heuristic is the taxicab distance between the square and the end point of the maze.
		std::vector<int> heuristic(data.width*data.height, -1);

		//initalize the openNodes list to contain the startNode and
		totalMovementCost[startNode] = 0;
		openList.push_back(startNode);

		//computes the heurisic values for each node
		for (int i = 0; i < heuristic.size(); i++)
		{
			heuristic[i] = calculateHeuristic(i, endX, endY, data.width);
		}

		while (!openList.empty())
		{
			int currentNode = openList.front();
			openList.pop_front();

			if (currentNode==endNode)
			{
				//do the end node stuff
				std::list<int> path;
				int parent = endNode;
				do
				{
					path.push_front(parent);
					parent = backNode[parent];
				} while (parent != -1);

				std::cout << "done solving maze. The path is " << path.size() << " pixels long." << std::endl;

				std::list<int>::iterator iter = path.begin();
				for (int i = 0; i < path.size(); i++)
				{
					unsigned char progress = (unsigned char)(255 * (i/(double)(path.size() - 1)));
					int colorNode = *iter;
					++iter;

					data.red[colorNode] = 255 - progress;
					data.green[colorNode] = 0;
					data.blue[colorNode] = progress;
				}

				return data;
			}

			//store the nodes that we will iterate through
			std::vector<int> explorationNodes;
			//if the node is not on the top line explore the node above the current node
			if (currentNode >= data.width)
			{
				explorationNodes.push_back(currentNode - data.width);
			}
			//if the node is not on the bottom line explore the node below the current node
			if (currentNode + data.width < data.width * data.height)
			{
				explorationNodes.push_back(currentNode + data.width);
			}
			//if the node is not the first of a line then explore the node to the left
			if (currentNode % data.width != 0)
			{
				explorationNodes.push_back(currentNode - 1);
			}
			//if the node is not the last of a line then explore the node to the right
			if (currentNode % data.width != data.width-1)
			{
				explorationNodes.push_back(currentNode + 1);
			}

			//iterate through the exploatory nodes
			for each (int epNode in explorationNodes)
			{
				if (data.red[epNode] == 255)
				{
					int distanceThroughPath = totalMovementCost[currentNode] + 1;
					if (totalMovementCost[epNode] == -1 || distanceThroughPath < totalMovementCost[epNode])
					{
						totalMovementCost[epNode] = distanceThroughPath;
						backNode[epNode] = currentNode;
						//if the epNode was already in the openList remove it so we can re-assign it in the proper position
						openList.remove(epNode);

						if (openList.size()!=0)
						{
							std::list<int>::iterator listpos = openList.begin();
							bool shouldPushBack = true;
							do
							{
								if (heuristic[epNode] + totalMovementCost[epNode] < heuristic[*listpos] + totalMovementCost[*listpos])
								{
									openList.insert(listpos, epNode);
									shouldPushBack = false;
									break;
								}
								++listpos;
							} while (listpos != openList.end());
							if (shouldPushBack)
							{
								openList.push_back(epNode);
							}
						}
						else
						{
							openList.push_front(epNode);
						}
					}
				}
			}
		}

		return data;
	}
	else
	{
		return data;
	}
}

MazeImageProcessor::~MazeImageProcessor()
{
}

int MazeImageProcessor::calculateHeuristic(int pixelNo, int endX, int endY, int width)
{
	return abs((pixelNo % width - endX)) + abs((pixelNo / width - endY));
}
