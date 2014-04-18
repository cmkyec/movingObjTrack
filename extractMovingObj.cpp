#include "extractMovingObj.h"
#include "disjointSet.h"
#include <iostream>

void inline expandRect(cv::Rect& rect, int imgWidth, int imgHeight)
{
	int top_left_x = (int)(rect.x - 15);
	if (top_left_x < 0)
		top_left_x = 0;
	int top_left_y = (int)(rect.y - 15);
	if (top_left_y < 0)
		top_left_y = 0;
	int bottom_right_x = (int)(rect.x + rect.width + 15);
	if (bottom_right_x >= imgWidth)
		bottom_right_x = imgWidth - 1;
	int bottom_right_y = (int)(rect.y + rect.height + 15);
	if (bottom_right_y >= imgHeight)
		bottom_right_y = imgHeight - 1;

	rect.x = top_left_x;
	rect.y = top_left_y;
	rect.width = bottom_right_x - top_left_x + 1;
	rect.height = bottom_right_y - top_left_y + 1;
}

bool rectOverlap(cv::Rect& a, cv::Rect& b)
{
	cv::Point top, bottom;
	top.x = std::max(a.x, b.x);
	top.y = std::max(a.y, b.y);
	bottom.x = std::min(a.x + a.width - 1, b.x + b.width - 1);
	bottom.y = std::min(a.y + a.height - 1, b.y + b.height - 1);

	return (bottom.y - top.y) >= 0 && (bottom.x - top.x) >= 0;
}

void objRects(std::vector<std::vector<cv::Point> >& contours,
	      std::vector<cv::Rect>& rects)
{
	std::vector<cv::Rect> rawRects;
	for (std::size_t i = 0; i < contours.size(); ++i) {
		cv::Rect r = cv::boundingRect(contours[i]);
		if (r.area() < 1000) continue;
		rawRects.push_back(r);
	}

	CDisjointSet ds((int)rawRects.size());
	for (std::size_t i = 0; i < rawRects.size(); ++i) {
		for (std::size_t j = i + 1; j < rawRects.size(); ++j) {
			if (rectOverlap(rawRects[i], rawRects[j])) {
				ds.merge((int)i, (int)j);
			}
		}
	}

	std::vector<std::vector<int> > sets;
	ds.subSet(sets);

	rects.clear();
	for (std::size_t i = 0; i < sets.size(); ++i) {
		std::vector<int>& s = sets[i];
		cv::Point top(100000, 100000), bottom(0, 0);
		for (std::size_t j = 0; j < s.size(); ++j) {
			int x = rawRects[s[j]].x;
			int y = rawRects[s[j]].y;
			int w = rawRects[s[j]].width;
			int h = rawRects[s[j]].height;
			if (x < top.x) top.x = x;
			if (y < top.y) top.y = y;
			if (x + w - 1 > bottom.x) bottom.x = x + w - 1;
			if (y + h - 1 > bottom.y) bottom.y = y + h - 1;
		}
		rects.push_back(cv::Rect(top, bottom));
	}
}

bool movingContourCheck(std::vector<std::vector<cv::Point> >& contours)
{
	unsigned int count = 0;
	for (std::size_t i = 0; i < contours.size(); ++i) {
		cv::Rect rect = cv::boundingRect(contours[i]);
		if (rect.area() < 1000) continue;
		count++;
	}
	return count < 15;
}

void extractMovingObj(std::string& videoPath, std::string& saveFolder)
{
	static cv::BackgroundSubtractorMOG2 mog(500, 16.0f, 1);
	cv::VideoCapture capture;
	if (!capture.open(videoPath)) {
		std::cerr << "can not open the video: " << videoPath << std::endl;
		return;
	}

	char savePath[256];
	memset(savePath, 0, sizeof(savePath));

	int nFrame = 1;
	cv::Mat frame, fgimg, bgimg;
	cv::namedWindow(saveFolder.c_str(), 0);
	cv::namedWindow("foreground", 0);
	while (true) {
		capture >> frame;
		if (!frame.data) break;
		mog(frame, fgimg);
		cv::threshold(fgimg, fgimg, 100, 255, CV_THRESH_BINARY);
		cv::medianBlur(fgimg, fgimg, 5);
		cv::imshow("foreground", fgimg);

		cv::Mat contour_img;
		fgimg.copyTo(contour_img);
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(contour_img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		std::vector<cv::Rect> rects;
		objRects(contours, rects);
		for (std::size_t i = 0; i < rects.size(); ++i) {
			expandRect(rects[i], frame.cols, frame.rows);
			cv::rectangle(frame, rects[i], cv::Scalar(0, 0, 255), 2);
		}
		
		cv::imshow(saveFolder.c_str(), frame);
		if (cv::waitKey(10) == 27) break; // 'esc' key break 
		nFrame++;
	}
	cv::destroyWindow(saveFolder.c_str());
}
