#include "Tracking.h"

Tracking::Tracking()
{
}

Tracking::~Tracking()
{
	TerminateITracking();
	Terminate();
}

std::string Tracking::IntToStr(int num)
{
	std::ostringstream s;
	s << num;
	return s.str();
}

bool Tracking::InitLibrary() {
	int errorCode;

	errorCode = IFACE_Init();
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
		return false;
	}
	else {
		cout << "initLibrary IFACE_Init OK " << endl;
		return true;
	}
		
}

void Tracking::InitITracking() {
	int errorCode;

	errorCode = IFACE_CreateFaceHandler(&faceHandlerTracking);	
	errorCode = IFACE_CreateObjectHandler(&objectHandler, faceHandlerTracking);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;		
	}

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++)
	{
		errorCode = IFACE_CreateObject(&objects[i]);		
	}

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_FACE_DISCOVERY_FREQUENCE_MS,
		IntToStr(refreshInterval).c_str());
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MIN_EYE_DISTANCE,
		IntToStr(minEyeDistance).c_str());
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MAX_EYE_DISTANCE,
		IntToStr(maxEyeDistance).c_str());
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(faceHandlerTracking,
		IFACE_PARAMETER_FACEDET_CONFIDENCE_THRESHOLD,
		IntToStr(faceConfidenceThresh).c_str()); //
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

	if (trackingMode == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_DEFAULT);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (trackingMode == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_LIVENESS_DOT);
		
	}
	if (trackingMode == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_OBJECT_TRACKING);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (trackSpeed == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_ACCURATE);
		
	}

	if (trackSpeed == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_BALANCED);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (trackSpeed == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_FAST);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (motionOptimization == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_DISABLED);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (motionOptimization == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_ACCURATE);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (motionOptimization == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_FAST);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (motionOptimization == 3)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_SHORT);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_DEEP_TRACK,
		deepTrack.c_str()); // IFACE_TRACK_DEEP_TRACK_DEFAULT
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

	flagTracking = false;
	flagFirstDetect = false;
}

unsigned char* Tracking::LoadImageOfMemory(vector<unsigned char> buffer,
	int *width, int *height) {
	int lenght, errorCode;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);
	if (imgData == NULL) {
		return NULL;
	}

	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), width,
		height, &lenght, NULL);
	if (errorCode != IFACE_OK) {		
		return NULL;
	}

	unsigned char* rawImage = new unsigned char[lenght];
	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), width,
		height, &lenght, rawImage);
	if (errorCode != IFACE_OK) {		
		return NULL;
	}

	return rawImage;

}

void Tracking::FaceTracking(char* data, int size) {
	int width, height, errorCode; 

	unsigned char* ucharData = reinterpret_cast<unsigned char*> (data);
	std::vector<unsigned char> vectorData(ucharData, ucharData + size);
	unsigned char* rawImageData = LoadImageOfMemory(vectorData, &width, &height);
	
	if (rawImageData != NULL) {

		errorCode = IFACE_TrackObjects(objectHandler, rawImageData,
			width, height, countFrameTracking*timeDeltaMs, NUM_TRACKED_OBJECTS, objects);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
		TrackObjectState();
		delete[] rawImageData;
	}
	flagTracking = false;
	vectorData.clear();

}

void Tracking::TrackObjectState() {
	int errorCode, countDesolation = 0;	

	for (int trackedObjectIndex = 0; trackedObjectIndex < NUM_TRACKED_OBJECTS;
		trackedObjectIndex++)
	{

		float bbX, bbY, bbWidth, bbHeight;
		IFACE_TrackedObjectState trackedState;

		errorCode = IFACE_GetObjectState(objects[trackedObjectIndex],
			objectHandler, &trackedState);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}

		if (trackedState == IFACE_TRACKED_OBJECT_STATE_CLEAN) {

			
			/*countDesolation++;
			if (countDesolation == NUM_TRACKED_OBJECTS && !flagFirstDetect)
			{
				AdvanceVideoStream();
			}*/
			//cout << "STATE_CLEAN" << endl;
			continue;
		}
				
		switch (trackedState)
		{
		case IFACE_TRACKED_OBJECT_STATE_TRACKED:
			//flagFirstDetect = true;
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			
			//BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
			printf("   face id is TRACKED. Its bounding box :(%f, %f), (%f, %f), Face score : , Object score : \n", bbX, bbY, bbWidth, bbHeight);
			break;
		case IFACE_TRACKED_OBJECT_STATE_COVERED:
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			printf("   face id is COVERED. Its bounding box :(%f, %f), (%f, %f), Face score : , Object score : \n", bbX, bbY, bbWidth, bbHeight);

			//BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
			break;
		case IFACE_TRACKED_OBJECT_STATE_SUSPEND:
			//ClearCoordinatesImage(trackedObjectIndex);
			printf("STATE SUSPEND INDEX: %d\n", trackedObjectIndex);

			break;
		case IFACE_TRACKED_OBJECT_STATE_LOST:
			//ClearCoordinatesImage(trackedObjectIndex);
			void *newObj;
			errorCode = IFACE_CreateObject(&newObj);
			objects[trackedObjectIndex] = newObj;
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			//flagFirstDetect = false;
			printf("STATE LOST INDEX: %d\n", trackedObjectIndex);

			break;
		case IFACE_TRACKED_OBJECT_STATE_CLEAN:
			printf("STATE LOST CLEAN OBJECT INDEX: %d\n", trackedObjectIndex);

			break;
		}

	}
	//countFrameTracking++;
}

string Tracking::GetMessageError(int errorCode) {
	const int errorBufferLength = BUFFER_LENGTH;
	char errorBuffer[errorBufferLength];
	char msg[CHAR_LENGTH];
	string msgError;

	int getErrorMsgErrorCode = IFACE_GetErrorMessage(errorCode,
		errorBufferLength, errorBuffer);
	if (getErrorMsgErrorCode == IFACE_OK)
	{
		sprintf_s(msg, "Error occurs! %s (code: %d)\n", errorBuffer, errorCode);
		msgError = msg;
	}
	else
	{
		sprintf_s(msg, "Error occurs! %s (code %d)\n", "Error occurred during error code to message translation", getErrorMsgErrorCode);
		msgError = msg;
	}

	return msgError;
}

void Tracking::TerminateITracking() {
	int errorCode;

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++)
	{
		try
		{
			if (objects[i] != NULL)
			{
				errorCode = IFACE_ReleaseEntity(objects[i]);
				if (errorCode != IFACE_OK) {
					cout << GetMessageError(errorCode).c_str() << endl;
				}
			}

		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
		}

	}

	errorCode = IFACE_ReleaseEntity(objectHandler);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

	errorCode = IFACE_ReleaseEntity(faceHandlerTracking);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

}

void Tracking::Terminate() {
	int errorCode;

	errorCode = IFACE_Terminate();
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	
}