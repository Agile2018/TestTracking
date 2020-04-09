#ifndef Video_h
#define Video_h

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

class Video
{
public:
	Video();
	~Video();

	void Play(cv::Mat frame);

	void SetDestroy(bool value) {
		destroy = value;
	}

	void SetNameWindow(string name) {
		nameWindow = name;
	}

private:
	bool destroy = false;
	string nameWindow = "frame";
};

#endif // !Video_h

