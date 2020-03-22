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
	if (tracking->InitLibrary())
	{
		tracking->SetMinEyeDistance(20);
		tracking->SetMaxEyeDistance(250);
		tracking->SetFaceConfidenceThresh(450);
		tracking->SetRefreshInterval(2000);
		tracking->SetSequenceFps(10);
		tracking->SetTrackingMode(2); //0:DEFAULT 1:LIVENESS_DOT 2:OBJECT_TRACKING
		tracking->SetTrackSpeed(1); //0:MODE_ACCURATE 1:MODE_BALANCED 2:MODE_FAST
		tracking->SetMotionOptimization(1); //0:DISABLED 1:HISTORY_LONG_ACCURATE 2:HISTORY_LONG_FAST 3:HISTORY_SHORT
		tracking->SetDeepTrack("true");
		tracking->InitITracking();
		gstreamer->SetTracking(tracking);
		gstreamer->SetFileVideo("video2.mp4");	
		//gstreamer->SetIpCamera("rtsp://root:admin@192.168.0.4:554/axis-media/media.amp");
		gstreamer->Playing(2);
	} 
	//****************
	
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
