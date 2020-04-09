#include "Video.h"

Video::Video()
{
}

Video::~Video()
{
}

void Video::Play(cv::Mat frame) {
	static int framecount = 0;

	if (!framecount) {
		cv::namedWindow(nameWindow.c_str(), cv::WINDOW_NORMAL);
		framecount++;
	}
	
	if (!frame.empty()) {		
		cv::imshow(nameWindow.c_str(), frame);
		cv::waitKey(1);
	}

	if (destroy)
	{
		cv::destroyWindow(nameWindow.c_str());
	}
}