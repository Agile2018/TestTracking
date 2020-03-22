#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <gst/gst.h>
#include <gst/app/app.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "Tracking.h"

using namespace std;

class Gstreamer
{
public:
	Gstreamer();
	~Gstreamer();
	void Playing(int optionInput);
	void SetIpCamera(string ip) {
		ipCamera = ip;
	}

	void SetDeviceVideo(string device) {
		deviceVideo = device;
	}

	void SetVideoScaleMethod(int value) {
		videoScaleMethod = value;
	}

	void SetWidthFrame(int value) {
		widthFrame = value;
	}

	void SetHeightFrame(int value) {
		heightFrame = value;
	}

	void SetFileVideo(string file) {
		fileVideo = file;
	}

	void SetTracking(Tracking* track);

private:
	string ipCamera;
	string fileVideo;
	string deviceVideo = "/dev/video0";
	int videoScaleMethod = 1; //3 1
	int widthFrame = 640;
	int heightFrame = 480;
	GstElement* pipeline;
	
	gchar* DescriptionPipeline(int optionInput);

};

#endif // !GSTREAMER_H

