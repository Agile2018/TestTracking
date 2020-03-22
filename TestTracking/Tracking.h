#ifndef TRACKING_H
#define TRACKING_H

#include <iostream>
#include <sstream>
#include <vector>
#include "iface.h"

#define BUFFER_LENGTH	1024
#define CHAR_LENGTH		256
#define NUM_TRACKED_OBJECTS	5

using namespace std;

class Tracking
{
public:
	Tracking();
	~Tracking();
	bool InitLibrary();
	void InitITracking();
	void FaceTracking(char* data, int size);
	void SetMinEyeDistance(int value) {
		minEyeDistance = value;
	}

	void SetMaxEyeDistance(int value) {
		maxEyeDistance = value;
	}

	void SetFaceConfidenceThresh(int value) {
		faceConfidenceThresh = value;
	}

	void SetTrackingMode(int mode) {
		trackingMode = mode;
	}

	void SetTrackSpeed(int speed) {
		trackSpeed = speed;
	}

	void SetMotionOptimization(int motion) {
		motionOptimization = motion;
	}

	void SetRefreshInterval(int value) {
		refreshInterval = value;
	}

	void SetDeepTrack(string value) {
		deepTrack = value;
	}

	void SetSequenceFps(int value) {
		sequenceFps = value;
		timeDeltaMs = 1000 / sequenceFps;
	}

private:
	int minEyeDistance = 20;          // minimal eye distance in input image
	int maxEyeDistance = 200;         // maximal eye distance in input image
	int faceConfidenceThresh = 450;         // face detection confidence threshold		
	int trackingMode = 1;
	int trackSpeed = 2;
	int motionOptimization = 2;
	string deepTrack = "true";
	int sequenceFps = 30;                   // fps of video	
	int timeDeltaMs = 1000 / sequenceFps;
	int countFrameTracking = 0;
	void* objectHandler;
	void* faceHandlerTracking;
	void* objects[NUM_TRACKED_OBJECTS];
	int refreshInterval = 2000;
	bool flagFirstDetect = false;
	bool flagTracking = false;
	string GetMessageError(int errorCode);
	string IntToStr(int num);
	unsigned char* LoadImageOfMemory(vector<unsigned char> buffer,
		int *width, int *height);
	void TrackObjectState();
	void TerminateITracking();
	void Terminate();
};


#endif // !TRACKING_H

