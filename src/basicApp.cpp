#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "OpenNI.h"

#include "OniSampleUtilities.h"


using namespace ci;
using namespace ci::app;
using namespace std;

using namespace openni;
#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms


class basicApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void basicApp::setup()
{
    Status rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        cout << "error code : 1" <<endl;
    }

    Device device;
    rc = device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        cout << "error code : 2" <<endl;
    }

    VideoStream depth;

    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
            cout << "error code : 3" <<endl;
        }
    }

    rc = depth.start();
    if (rc != STATUS_OK)
    {
        printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
        cout << "error code : 4" <<endl;
    }

    VideoFrameRef frame;

    while (!wasKeyboardHit())
    {
        int changedStreamDummy;
        VideoStream* pStream = &depth;
        rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
        if (rc != STATUS_OK)
        {
            printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
            continue;
        }

        rc = depth.readFrame(&frame);
        if (rc != STATUS_OK)
        {
            printf("Read failed!\n%s\n", OpenNI::getExtendedError());
            continue;
        }

        if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
        {
            printf("Unexpected frame format\n");
            continue;
        }

        DepthPixel* pDepth = (DepthPixel*)frame.getData();

        int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;

        printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
    }

    depth.stop();
    depth.destroy();
    device.close();
    OpenNI::shutdown();

    
}

void basicApp::mouseDown( MouseEvent event )
{
}

void basicApp::update()
{
}

void basicApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( basicApp, RendererGl )
