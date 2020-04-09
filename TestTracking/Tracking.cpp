#include "Tracking.h"

float imageCoordinatesFollowed[COORDINATES_X_ALL_IMAGES] = {};
double colorRectangle[NUM_TRACKED_OBJECTS] = {};

Tracking::Tracking()
{
	isGraphicProcessor = graphicProcessor->ThereIsGraphicProcessor();
	manageFile->SetNameDirectory(directory);
	manageFile->SetNameFile(nameFile);
}

Tracking::~Tracking()
{
	TerminateITracking();
	Terminate();
}

bool Tracking::InitParamsGraphicProcessor() {
	int errorCode;
	errorCode = IFACE_SetParam(nullptr,
		IFACE_PARAMETER_GLOBAL_GPU_ENABLED, "true");
	if (errorCode == IFACE_OK) {
		errorCode = IFACE_SetParam(nullptr,
			IFACE_PARAMETER_GLOBAL_GPU_DEVICE_ID, "0");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
			return false;
		}
	}
	else {
		cout << GetMessageError(errorCode).c_str() << endl;
		return false;
	}
	return true;
}

void Tracking::SetParamsLibrary() {
	int errorCode;
	string description = "";
	if (isGraphicProcessor) {
		if (InitParamsGraphicProcessor())
		{
			cout << "CUDA SET INNOVATRICS OK." << endl;
			description += format->FormatString("CUDA SET INNOVATRICS OK.\n");
		}
		else {
			cout << "CUDA SET INNOVATRICS ERROR." << endl;
			description += format->FormatString("CUDA SET INNOVATRICS ERROR.\n");
		}
	}
	else {
		cout << "CUDA NO EXISTE O NO HA SIDO SELECCIONADO." << endl;
		description += format->FormatString("CUDA does not exist or has not been selected.\n");
	}
	//  IFACE_GLOBAL_THREAD_MANAGEMENT_MODE_MIN_MEMORY 
	// IFACE_GLOBAL_THREAD_MANAGEMENT_MODE_MAX_PARALLEL
	//  IFACE_GLOBAL_THREAD_MANAGEMENT_MODE_SINGLE
	errorCode = IFACE_SetParam(IFACE_GLOBAL_PARAMETERS,
		IFACE_PARAMETER_GLOBAL_THREAD_MANAGEMENT_MODE, 
		IFACE_GLOBAL_THREAD_MANAGEMENT_MODE_MIN_MEMORY);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(IFACE_GLOBAL_PARAMETERS,
		IFACE_PARAMETER_GLOBAL_THREAD_NUM, "4"); //IFACE_GLOBAL_THREAD_NUM_DEFAULT
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

	manageFile->WriteFile(description);
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
	string description = "Params Tracking\n";
	errorCode = IFACE_CreateFaceHandler(&faceHandlerTracking);	
	errorCode = IFACE_CreateObjectHandler(&objectHandler, faceHandlerTracking);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;		
	}

	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++)
	{
		errorCode = IFACE_CreateObject(&objects[i]);		
	}
	description += format->FormatString("Num Tracked Objects:  %d.\n", NUM_TRACKED_OBJECTS);
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_FACE_DISCOVERY_FREQUENCE_MS,
		IntToStr(refreshInterval).c_str());
	description += format->FormatString("IFACE_PARAMETER_TRACK_FACE_DISCOVERY_FREQUENCE_MS:  %d.\n", refreshInterval);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MIN_EYE_DISTANCE,
		IntToStr(minEyeDistance).c_str());
	description += format->FormatString("IFACE_PARAMETER_TRACK_MIN_EYE_DISTANCE:  %d.\n", minEyeDistance);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MAX_EYE_DISTANCE,
		IntToStr(maxEyeDistance).c_str());
	description += format->FormatString("IFACE_PARAMETER_TRACK_MAX_EYE_DISTANCE:  %d.\n", maxEyeDistance);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	errorCode = IFACE_SetParam(faceHandlerTracking,
		IFACE_PARAMETER_FACEDET_CONFIDENCE_THRESHOLD,
		IntToStr(faceConfidenceThresh).c_str()); //
	description += format->FormatString("IFACE_PARAMETER_FACEDET_CONFIDENCE_THRESHOLD:  %d.\n", faceConfidenceThresh);
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}

	if (trackingMode == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_DEFAULT);
		description += format->FormatString("IFACE_PARAMETER_TRACK_TRACKING_MODE:  %s.\n", "IFACE_TRACK_TRACKING_MODE_DEFAULT");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (trackingMode == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_LIVENESS_DOT);
		description += format->FormatString("IFACE_PARAMETER_TRACK_TRACKING_MODE:  %s.\n", "IFACE_TRACK_TRACKING_MODE_LIVENESS_DOT");
	}
	if (trackingMode == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_TRACKING_MODE,
			IFACE_TRACK_TRACKING_MODE_OBJECT_TRACKING);
		description += format->FormatString("IFACE_PARAMETER_TRACK_TRACKING_MODE:  %s.\n", "IFACE_TRACK_TRACKING_MODE_OBJECT_TRACKING");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (trackSpeed == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_ACCURATE);
		description += format->FormatString("IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE:  %s.\n", "IFACE_TRACK_SPEED_ACCURACY_MODE_ACCURATE");
	}

	if (trackSpeed == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_BALANCED);
		description += format->FormatString("IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE:  %s.\n", "IFACE_TRACK_SPEED_ACCURACY_MODE_BALANCED");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (trackSpeed == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
			IFACE_TRACK_SPEED_ACCURACY_MODE_FAST);
		description += format->FormatString("IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE:  %s.\n", "IFACE_TRACK_SPEED_ACCURACY_MODE_FAST");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (motionOptimization == 0)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_DISABLED);
		description += format->FormatString("IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION:  %s.\n", "IFACE_TRACK_MOTION_OPTIMIZATION_DISABLED");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	if (motionOptimization == 1)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_ACCURATE);
		description += format->FormatString("IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION:  %s.\n", "IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_ACCURATE");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (motionOptimization == 2)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_FAST);
		description += format->FormatString("IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION:  %s.\n", "IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_FAST");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}
	if (motionOptimization == 3)
	{
		errorCode = IFACE_SetParam(objectHandler,
			IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
			IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_SHORT);
		description += format->FormatString("IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION:  %s.\n", "IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_SHORT");
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
	}

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_DEEP_TRACK,
		deepTrack.c_str()); // IFACE_TRACK_DEEP_TRACK_DEFAULT
	description += format->FormatString("IFACE_PARAMETER_TRACK_DEEP_TRACK:  %s.\n", deepTrack.c_str());
	if (errorCode != IFACE_OK) {
		cout << GetMessageError(errorCode).c_str() << endl;
	}
	sizeVideoStream = refreshInterval / timeDeltaMs;
	flagTracking = false;
	flagFirstDetect = false;
	manageFile->WriteFile(description);
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

void ClearCoordinatesImage(int indexTracked) {
	int index = indexTracked * NUM_COORDINATES_X_IMAGE;
	imageCoordinatesFollowed[index] = 0;
	imageCoordinatesFollowed[index + 1] = 0;
	imageCoordinatesFollowed[index + 2] = 0;
	imageCoordinatesFollowed[index + 3] = 0;
}

void BuildCoordinatesImage(float x, float y, float width, float height, int indexTracked) {
	int index = indexTracked * NUM_COORDINATES_X_IMAGE;
	imageCoordinatesFollowed[index] = x;
	imageCoordinatesFollowed[index + 1] = y;
	imageCoordinatesFollowed[index + 2] = width;
	imageCoordinatesFollowed[index + 3] = height;
}

float* Tracking::GetCoordiantesRectangle() {
	return imageCoordinatesFollowed;
}

double* Tracking::GetColorRectangle() {
	return colorRectangle;
}

void Tracking::AdvanceVideoStream() {	
	int positionVideoStream = (countFrameTracking / sizeVideoStream) + 1;
	int positionFrameMaxVideoStream = sizeVideoStream * positionVideoStream;

	if (countFrameTracking < positionFrameMaxVideoStream)
	{
		countFrameTracking = positionFrameMaxVideoStream;
	}
}

void ClearAllCoordinatesImage() {
	for (int i = 0; i < NUM_TRACKED_OBJECTS; i++) {
		ClearCoordinatesImage(i);		
	}
}

void Tracking::ResetCoordinates() {
	long countFrames = countFrameTracking - 1;	
	int residue = countFrames % sizeVideoStream;
	if (residue == 0)
	{
		ClearAllCoordinatesImage();
		
	}


}

void SetColorRectangle(float score, int indexObject) {
	double percentageScore = ((double)score * 100.0) / 10000.0;
	double variationColor = (255.0 * percentageScore) / 100.0;
	double color = (255.0 - variationColor);
	colorRectangle[indexObject] = color;
}

void Tracking::FaceTracking(std::vector<unsigned char> vectorData) {
	int width, height, errorCode; 
	string description = "New tracked object............\n";
	clock_t timeStart2 = clock();
	unsigned char* rawImageData = LoadImageOfMemory(vectorData, &width, &height);
	clock_t duration2 = clock() - timeStart2;
	int durationMs2 = int(1000 * ((float)duration2) / CLOCKS_PER_SEC);
	description += format->FormatString("Loading Image time: %d \n", durationMs2);
	//printf("   Loading Image time: %d \n", durationMs2);


	if (rawImageData != NULL) {
		long secuence = countFrameTracking * timeDeltaMs;
		ResetCoordinates();
		clock_t timeStart1 = clock();
		errorCode = IFACE_TrackObjects(objectHandler, rawImageData,
			width, height, secuence, NUM_TRACKED_OBJECTS, objects);
		clock_t duration1 = clock() - timeStart1;
		int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
		description += format->FormatString("Tracking Object time: %d \n", durationMs1);
		//printf("   TRACKING OBJECT time: %d \n", durationMs1);

		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
		TrackObjectState(description);
		delete[] rawImageData;
	}
	flagTracking = false;
	vectorData.clear();

}

void Tracking::TrackObjectState(string descriptionTimes) {
	int errorCode, countDesolation = 0;	
	bool track = false;
	string description = descriptionTimes;
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
			
			countDesolation++;
			if (countDesolation == NUM_TRACKED_OBJECTS && !flagFirstDetect)
			{
				AdvanceVideoStream();
			}
			
			errorCode = IFACE_CleanObject(objects[trackedObjectIndex], objectHandler);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			continue;
		}
				
		switch (trackedState)
		{
		case IFACE_TRACKED_OBJECT_STATE_TRACKED:
			flagFirstDetect = true;
			float objectScore;
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}			
			BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);

			errorCode = IFACE_GetObjectScore(objects[trackedObjectIndex], 
				objectHandler, &objectScore);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			SetColorRectangle(objectScore, trackedObjectIndex);
			description += format->FormatString("face is TRACKED. Its bounding box :(%f, %f), (%f, %f)\n", bbX, bbY, bbWidth, bbHeight);
			track = true;
			//printf("   face id is TRACKED. Its bounding box :(%f, %f), (%f, %f), Face score : , Object score : \n", bbX, bbY, bbWidth, bbHeight);
			break;
		case IFACE_TRACKED_OBJECT_STATE_COVERED:
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			//printf("   face id is COVERED. Its bounding box :(%f, %f), (%f, %f), Face score : , Object score : \n", bbX, bbY, bbWidth, bbHeight);

			BuildCoordinatesImage(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
			description += format->FormatString("face is COVERED. Its bounding box :(%f, %f), (%f, %f)\n", bbX, bbY, bbWidth, bbHeight);
			track = true;
			break;
		case IFACE_TRACKED_OBJECT_STATE_SUSPEND:
			ClearCoordinatesImage(trackedObjectIndex);
			errorCode = IFACE_CleanObject(objects[trackedObjectIndex], objectHandler);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			//printf("STATE SUSPEND INDEX: %d\n", trackedObjectIndex);
			description += format->FormatString("STATE SUSPEND INDEX: %d\n", trackedObjectIndex);
			
			break;
		case IFACE_TRACKED_OBJECT_STATE_LOST:
			ClearCoordinatesImage(trackedObjectIndex);
			errorCode = IFACE_CleanObject(objects[trackedObjectIndex], objectHandler);
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			
			void *newObj;
			errorCode = IFACE_CreateObject(&newObj);
			objects[trackedObjectIndex] = newObj;
			if (errorCode != IFACE_OK) {
				cout << GetMessageError(errorCode).c_str() << endl;
			}
			flagFirstDetect = false;
			//printf("STATE LOST INDEX: %d\n", trackedObjectIndex);
			description += format->FormatString("STATE LOST INDEX: %d\n", trackedObjectIndex);
			break;
		case IFACE_TRACKED_OBJECT_STATE_CLEAN:
			printf("STATE LOST 1 cCLEAN OBJECT INDEX: %d\n", trackedObjectIndex);

			break;
		}
		/*std::thread squ(&Tracking::SaveInformationTracking, this, 
			description, trackedObjectIndex, track);
		squ.detach();*/
		
	}
	countFrameTracking++;
}

void Tracking::SaveInformationTracking(string descriptionBefore,
	int indexObject, bool track) {
	int errorCode;
	string description = descriptionBefore;
	if (track)
	{
		void* face;
		errorCode = IFACE_CreateFace(&face);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}


		errorCode = IFACE_GetFaceFromObject(objects[indexObject], 
			objectHandler, face, IFACE_TRACKED_OBJECT_FACE_TYPE_LAST_DISCOVERY);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}

		float rightEyeX, rightEyeY, leftEyeX, leftEyeY;
		float faceScore;
		errorCode = IFACE_GetFaceBasicInfo(face, faceHandlerTracking,
			&rightEyeX, &rightEyeY, &leftEyeX, &leftEyeY, &faceScore);
		if (errorCode != IFACE_OK) {
			cout << GetMessageError(errorCode).c_str() << endl;
		}
		else {
			description += format->FormatString("rightEyeX: %.0f\n", rightEyeX);
			description += format->FormatString("rightEyeY: %.0f\n", rightEyeY);
			description += format->FormatString("leftEyeX: %.0f\n", leftEyeX);
			description += format->FormatString("leftEyeY: %.0f\n", leftEyeY);
			description += format->FormatString("Score: %.0f\n", faceScore);
		}
	}
	manageFile->WriteFile(description);
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