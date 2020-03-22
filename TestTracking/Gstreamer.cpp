#include "Gstreamer.h"

cv::Mat frame;
GMainLoop *loop;
Tracking* tracking = new Tracking();

Gstreamer::Gstreamer()
{
}

Gstreamer::~Gstreamer()
{
}

void Gstreamer::SetTracking(Tracking* track) {
	tracking = track;
}

static gboolean MessageErrorCallback(GstBus *bus, GstMessage *message, gpointer data)
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


	GstMapInfo map_info;


	if (!gst_buffer_map((buffer), &map_info, GST_MAP_READ)) {
		gst_buffer_unmap((buffer), &map_info);
		gst_sample_unref(sample);
		return GST_FLOW_ERROR;
	}

	//render using map_info.data
	/*frame = cv::Mat(height, width, CV_8UC3, (char *)map_info.data, cv::Mat::AUTO_STEP);

	std::thread tti(ThreadMatOnBuffer, frame);
	tti.detach(); */

	tracking->FaceTracking((char*)map_info.data, (int)map_info.size);
	
	std::vector<uchar> bufferMap((char*)map_info.data, (char*)map_info.data + (int)map_info.size);
		
	clock_t timeStart1 = clock();
	frame = cv::imdecode(bufferMap, cv::IMREAD_UNCHANGED);
	clock_t duration1 = clock() - timeStart1;
	int durationMs1 = int(1000 * ((float)duration1) / CLOCKS_PER_SEC);
	printf("   cv::imdecode conversion time: %d \n", durationMs1);

	//memcpy(frame.data, map_info.data, map_info.size);

	if (!frame.empty())
		cv::imshow("test-gstreamer-video", frame);
	cv::waitKey(1);


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
			"! decodebin ! videoconvert n-threads=2 "
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
			"! decodebin ! videoconvert n-threads=2 "
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
			"! decodebin ! videoconvert n-threads=2 "
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


