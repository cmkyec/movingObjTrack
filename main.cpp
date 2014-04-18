#include <iostream>
#include <vector>
#include "disjointSet.h"
#include "utility.h"
#include "extractMovingObj.h"

int main()
{
	std::string videoPath("C:/work/laoshang_video/video_429_10-39.src.mpg");
	std::string saveFolder("./test");

	extractMovingObj(videoPath, saveFolder);

        return 0;
}
