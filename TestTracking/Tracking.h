#ifndef TRACKING_H
#define TRACKING_H

#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include "iface.h"
#include "GraphicProcessor.h"
#include "File.h"
#include "Format.h"
#include <thread>

#define BUFFER_LENGTH	1024
#define CHAR_LENGTH		256
#define NUM_TRACKED_OBJECTS	5
#define NUM_COORDINATES_X_IMAGE  4
#define COORDINATES_X_ALL_IMAGES 20

using namespace std;

class Tracking
{
public:
	Tracking();
	~Tracking();
	bool InitLibrary();
	void InitITracking();
	void FaceTracking(std::vector<unsigned char> vectorData);
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
		if (value != 0)
		{
			sequenceFps = value;
			timeDeltaMs = 1000 / sequenceFps;
		}
		
	}

	bool GetFlagTracking() {
		return flagTracking;
	}

	void SetFlagTracking(bool value) {
		flagTracking = value;
	}
	void SetIsGraphicProcessor(bool processor) {
		isGraphicProcessor = processor;
	}

	float* GetCoordiantesRectangle();
	double* GetColorRectangle();
	void SetParamsLibrary();
private:
	const string directory = "Log";
	const string nameFile = "log.txt";
	int minEyeDistance = 20;          // minimal eye distance in input image
	int maxEyeDistance = 200;         // maximal eye distance in input image
	int faceConfidenceThresh = 450;         // face detection confidence threshold		
	int trackingMode = 1;
	int trackSpeed = 2;
	int motionOptimization = 2;
	string deepTrack = "true";
	int sequenceFps = 30;                   // fps of video	
	int timeDeltaMs = 1000 / sequenceFps;
	long countFrameTracking = 0;
	int sizeVideoStream = 0;
	void* objectHandler = nullptr;
	void* faceHandlerTracking = nullptr;
	void* objects[NUM_TRACKED_OBJECTS] = {};
	bool isGraphicProcessor = false;
	int refreshInterval = 2000;
	bool flagFirstDetect = false;
	bool flagTracking = false;
	GraphicProcessor* graphicProcessor = new GraphicProcessor();
	File* manageFile = new File();
	Format* format = new Format();
	string GetMessageError(int errorCode);
	string IntToStr(int num);
	unsigned char* LoadImageOfMemory(vector<unsigned char> buffer,
		int *width, int *height);
	void AdvanceVideoStream();
	bool InitParamsGraphicProcessor();
	void ResetCoordinates();
	void TrackObjectState(string descriptionTimes);
	void SaveInformationTracking(string descriptionBefore,
		int indexObject, bool track);
	void TerminateITracking();
	void Terminate();
};


#endif // !TRACKING_H

