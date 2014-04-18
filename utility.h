#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
#include <vector>

bool extractName(std::string& videoPath, std::string& videoName);

bool readVideoPaths(std::string textFile, std::vector<std::string>& paths);


#endif /* _UTILITY_H_ */