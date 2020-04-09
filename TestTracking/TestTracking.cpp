// TestTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>
#include "Gstreamer.h"
#include "Tracking.h"

int main()
{
	Tracking* tracking = new Tracking();
	Gstreamer* gstreamer = new Gstreamer();

	//****************
	//tracking->SetIsGraphicProcessor(false);
	tracking->SetParamsLibrary();
	if (tracking->InitLibrary())
	{
		tracking->SetMinEyeDistance(20);
		tracking->SetMaxEyeDistance(250);
		tracking->SetFaceConfidenceThresh(450);
		tracking->SetRefreshInterval(2000);
		tracking->SetSequenceFps(10);
		tracking->SetTrackingMode(2); //0:DEFAULT 1:LIVENESS_DOT 2:OBJECT_TRACKING
		tracking->SetTrackSpeed(2); //0:MODE_ACCURATE 1:MODE_BALANCED 2:MODE_FAST
		tracking->SetMotionOptimization(1); //0:DISABLED 1:HISTORY_LONG_ACCURATE 2:HISTORY_LONG_FAST 3:HISTORY_SHORT
		tracking->SetDeepTrack("true");		
		tracking->InitITracking();		
		gstreamer->SetTracking(tracking);
		gstreamer->SetFileVideo("camargo.mp4");	
		//gstreamer->SetIpCamera("rtsp://root:admin@192.168.0.4:554/axis-media/media.amp");
		gstreamer->Playing(2);
	} 
	//****************
	
	return 0;
}

