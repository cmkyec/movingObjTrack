#include "utility.h"
#include <iostream>
#include <fstream>

bool extractName(std::string& videoPath, std::string& videoName)
{
	std::size_t dotPos = videoPath.rfind(".");
	if (dotPos == std::string::npos) {
		std::cerr << "can not find a dot in the path: "
			  << videoPath << std::endl;
		return false;
	}

	std::size_t backslashPos = videoPath.rfind("/");
	if (backslashPos == std::string::npos) {
		videoName = videoPath.substr(0, dotPos);
	}
	else {
		videoName = videoPath.substr(backslashPos + 1,
			dotPos - backslashPos - 1);
	}
	return true;
}

bool readVideoPaths(std::string textFile, std::vector<std::string>& paths)
{
	std::ifstream file(textFile.c_str());
	if (!file.is_open()) {
		std::cerr << "can not open the text file: "
			  << textFile << std::endl;
		return false;
	}

	std::string path;
	while (std::getline(file, path, '\n')) {
		paths.push_back(path);
	}
	return true;
}



