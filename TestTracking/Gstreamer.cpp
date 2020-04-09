#include "Gstreamer.h"

cv::Mat frame;
GMainLoop *loop;
Tracking* tracking = new Tracking();
static Video* video = new Video();

Gstreamer::Gstreamer()
{
}

Gstreamer::~Gstreamer()
{
}

void Gstreamer::SetTracking(Tracking* track) {
	tracking = track;
}

gboolean MessageErrorCallback(GstBus *bus, GstMessage *message, gpointer data)
{
	
	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_ERROR: {
		GError *err;
		gchar *debug;

		gst_message_parse_error(message, &err, &debug);
		g_print("Error: %s\n", err->message);
		g_error_free(err);
		g_free(debug);
		break;
	}
	case GST_MESSAGE_EOS:
		/* end-of-stream */
		g_print("End: %s\n", "end-of-stream");		
		g_main_loop_quit(loop);
		break;
	default:
		/* unhandled message */
		//g_print("Unhandled: %s\n", "unhandled message");

		break;
	}

	return true;
}

std::vector<uchar> WriteMatOnBuffer(cv::Mat frame) {

	std::vector<uchar> bufferImage;
	int params[3] = { 0 };
	params[0] = cv::IMWRITE_JPEG_QUALITY;
	params[1] = 100;
	if (!frame.empty())
	{
		bool code = cv::imencode(".jpg", frame,
			bufferImage, std::vector<int>(params, params + 2));

	}
	return bufferImage;

}

void ThreadMatOnBuffer(cv::Mat frame) {
	clock_t timeStart1 = clock();
	vector<uchar> buf = WriteMatOnBuffer(frame);
	clock_t duration1 = clock() - timeStart1;
	int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
	printf("   Mat to Buffer conversion time: %d \n", durationMs1);
}

void RunTracking(std::vector<uchar> buffer) {
	if (!tracking->GetFlagTracking())
	{
		tracking->SetFlagTracking(true);
		tracking->FaceTracking(buffer);
	}
}

void DrawRectangles(cv::Mat workMat) {
	static bool flagPaint[NUM_TRACKED_OBJECTS] = {};
	static int area[NUM_TRACKED_OBJECTS] = {};
	float* coordinatesFace = tracking->GetCoordiantesRectangle();
	double* colorLine = tracking->GetColorRectangle();

	for (int i = 0; i < COORDINATES_X_ALL_IMAGES; i += 4)
	{
		if (coordinatesFace[i] != 0) {
			int x = (int)coordinatesFace[i];
			int y = (int)coordinatesFace[i + 1];
			int w = (int)coordinatesFace[i + 2];
			int h = (int)coordinatesFace[i + 3];			
			double c = colorLine[i];
			
			int horizontal[6] = {};
			int vertical[6] = {};
			int segmentWidth = w / 5;
			int segmentHeight = h / 5;
			for (int i = 0; i < 6; i++)
			{
				if (i == 0)
				{
					horizontal[i] = x;
				}
				else {
					horizontal[i] = horizontal[i - 1] + segmentWidth;
				}
				
			}

			for (int i = 0; i < 6; i++)
			{
				if (i == 0)
				{
					vertical[i] = y;
				}
				else {
					vertical[i] = vertical[i - 1] + segmentHeight;
				}

			}		

			cv::Point p1u(horizontal[0], vertical[0]), p2u(horizontal[1], vertical[0]),
				p3u(horizontal[2], vertical[0]), p4u(horizontal[3], vertical[0]),
				p5u(horizontal[4], vertical[0]), p6u(horizontal[5], vertical[0]);

			cv::Point p1l(horizontal[0], vertical[0]), p2l(horizontal[0], vertical[1]),
				p3l(horizontal[0], vertical[2]), p4l(horizontal[0], vertical[3]),
				p5l(horizontal[0], vertical[4]), p6l(horizontal[0], vertical[5]);

			cv::Point p1d(horizontal[0], vertical[0] + h), p2d(horizontal[1], vertical[0] + h),
				p3d(horizontal[2], vertical[0] + h), p4d(horizontal[3], vertical[0] + h),
				p5d(horizontal[4], vertical[0] + h), p6d(horizontal[5], vertical[0] + h);

			cv::Point p1r(horizontal[0] + w, vertical[0]), p2r(horizontal[0] + w, vertical[1]),
				p3r(horizontal[0] + w, vertical[2]), p4r(horizontal[0] + w, vertical[3]),
				p5r(horizontal[0] + w, vertical[4]), p6r(horizontal[0] + w, vertical[5]);

			if (area[i] != (w * h))
			{
				flagPaint[i] = !flagPaint[i];
				area[i] = w * h;
			}

			if (flagPaint[i])
			{
				cv::line(workMat, p1u, p2u, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p3u, p4u, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p5u, p6u, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p1l, p2l, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p3l, p4l, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p5l, p6l, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p1d, p2d, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p3d, p4d, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p5d, p6d, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p1r, p2r, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p3r, p4r, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p5r, p6r, cv::Scalar(255.0, 255.0, c), 2);
			}
			else {
				cv::line(workMat, p2u, p3u, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p4u, p5u, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p2l, p3l, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p4l, p5l, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p2d, p3d, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p4d, p5d, cv::Scalar(255.0, 255.0, c), 2);

				cv::line(workMat, p2r, p3r, cv::Scalar(255.0, 255.0, c), 2);
				cv::line(workMat, p4r, p5r, cv::Scalar(255.0, 255.0, c), 2);
			}
			
			
		}
	}
	
}

GstFlowReturn CaptureGstBuffer(GstAppSink *sink, gpointer user_data)
{
	static int framecount = 0;
	GstSample* sample = gst_app_sink_pull_sample(sink);

	if (sample == NULL) {
		return GST_FLOW_ERROR;
	}
	GstCaps *caps = gst_sample_get_caps(sample);
	GstBuffer* buffer = gst_sample_get_buffer(sample);
	GstStructure *structure = gst_caps_get_structure(caps, 0);
	const int width = g_value_get_int(gst_structure_get_value(structure, "width"));
	const int height = g_value_get_int(gst_structure_get_value(structure, "height"));

	// Show caps on first frame
	if (!framecount) {
		g_print("caps: %s\n", gst_caps_to_string(caps));
	}
	framecount++;
	/*if (framecount == 50)
	{
		video->SetDestroy(true);
	}*/

	GstMapInfo map_info;


	if (!gst_buffer_map((buffer), &map_info, GST_MAP_READ)) {
		gst_buffer_unmap((buffer), &map_info);
		gst_sample_unref(sample);
		return GST_FLOW_ERROR;
	}
		
	std::vector<uchar> bufferMap((char*)map_info.data, (char*)map_info.data + (int)map_info.size);
	
	std::thread ttb(RunTracking, bufferMap);
	ttb.detach();

	try
	{
		//clock_t timeStart1 = clock();
		frame = cv::imdecode(bufferMap, cv::IMREAD_UNCHANGED);
		/*clock_t duration1 = clock() - timeStart1;
		int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
		printf("   cv::imdecode conversion time: %d \n", durationMs1);*/

		//memcpy(frame.data, map_info.data, map_info.size);

		if (!frame.empty()) {
			
			DrawRectangles(frame);
			video->Play(frame);
			/*cv::imshow("test-gstreamer-video", frame);
			cv::waitKey(1);*/
		}
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}
			
	gst_buffer_unmap((buffer), &map_info);

	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

void Gstreamer::Playing(int optionInput) {
	GError *gError = nullptr;
	GstStateChangeReturn ret;
	gchar *descr = DescriptionPipeline(optionInput);
	gst_init(NULL, NULL);
	pipeline = gst_parse_launch(descr, &gError);
	if (gError) {
		g_print("could not construct pipeline: %s\n", gError->message);
		g_error_free(gError);
		exit(-1);
	}
	GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
	gst_app_sink_set_emit_signals((GstAppSink*)sink, true);
	gst_app_sink_set_drop((GstAppSink*)sink, true);
	gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);

	GstAppSinkCallbacks* appsink_callbacks = (GstAppSinkCallbacks*)malloc(sizeof(GstAppSinkCallbacks));
	appsink_callbacks->eos = NULL;
	appsink_callbacks->new_preroll = NULL;
	appsink_callbacks->new_sample = CaptureGstBuffer;
	gst_app_sink_set_callbacks(GST_APP_SINK(sink), appsink_callbacks, (gpointer)NULL, free);

	/* Wait until error or EOS */
	GstBus *bus;
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, MessageErrorCallback, nullptr);
	gst_object_unref(bus);

	/* Start playing */
	ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(pipeline);
		exit(-1);
	}

	loop = g_main_loop_new(NULL, false);
	g_main_loop_run(loop);

	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));

}

gchar* Gstreamer::DescriptionPipeline(int optionInput) {
	gchar *descr = nullptr;

	switch (optionInput) {
	case 1: // IP CAMERA
		descr = g_strdup_printf(
			"rtspsrc location=%s "
			"! application/x-rtp, payload=96 ! rtph264depay ! h264parse ! avdec_h264 "
			"! decodebin ! videoconvert "
			"! video/x-raw, format=(string)I420 "
			"! jpegenc "
			"! appsink name=sink emit-signals=true sync=false max-buffers=1 drop=true",
			ipCamera.c_str()
		);
		/*descr = g_strdup_printf(
			"rtspsrc location=%s "
			"! application/x-rtp, payload=96 ! rtph264depay ! h264parse ! avdec_h264 "
			"! decodebin ! videoconvert n-threads=4 ! videoscale method=%d ! videorate "
			"! video/x-raw, width=(int)%d, height=(int)%d, format=(string)I420, framerate=30/1 "
			"! jpegenc quality=40 "
			"! appsink name=sink emit-signals=true sync=false max-buffers=1 drop=true",
			ipCamera.c_str(), videoScaleMethod, widthFrame, heightFrame
		);  */

		// latency=0  drop-on-latency=true width=(int)%d, height=(int)%d, 
		break;
	case 2: // FILE		
		descr = g_strdup_printf(
			"filesrc location=%s "
			"! decodebin ! videoconvert "
			"! video/x-raw, format=(string)I420 "
			"! jpegenc quality=100 "
			"! appsink name=sink emit-signals=true sync=true max-buffers=1 drop=true",
			fileVideo.c_str()
		);
		/*descr = g_strdup_printf(
			"filesrc location=%s "
			"! decodebin ! videoconvert n-threads=4 ! videoscale method=%d "
			"! videobalance contrast=1 brightness=0 saturation=1 "
			"! video/x-raw, width=(int)%d, height=(int)%d, format=(string)I420"
			"! jpegenc "
			"! appsink name=sink emit-signals=true sync=true max-buffers=1 drop=true",
			fileVideo.c_str(), videoScaleMethod, widthFrame, heightFrame
		);*/

		// "! jpegenc quality=20 " pngenc RGB , format=(string)I420 BGR YUY2
		break;
	case 3: // CAMERA   
		descr = g_strdup_printf(
			"v4l2src device=%s "
			"! decodebin ! videoconvert "
			"! video/x-raw, format=(string)I420  "
			"! jpegenc "
			"! appsink name=sink emit-signals=true sync=true max-buffers=1 drop=true",
			deviceVideo.c_str()
		);

		/*descr = g_strdup_printf(
			"v4l2src device=%s "
			"! decodebin ! videoconvert n-threads=4 ! videoscale method=%d "
			"! videobalance contrast=1 brightness=0 saturation=1 "
			"! video/x-raw, width=(int)%d, height=(int)%d, format=(string)I420  "
			"! jpegenc quality=40 "
			"! appsink name=sink emit-signals=true sync=true max-buffers=1 drop=true",
			deviceVideo.c_str(), videoScaleMethod, widthFrame, heightFrame
		); */
		break;
	}
	return descr;
}


