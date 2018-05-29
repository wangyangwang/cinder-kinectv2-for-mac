#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
//#include "OpenNI.h"

#include "NiTE.h"


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

    nite::NiTE::initialize();

    nite::UserTracker userTracker;

    nite::Status niteRc = userTracker.create();

    if (niteRc != nite::STATUS_OK) {
        cout << "Couldn't create user tracker";
    }
    else {
        cout << "NiTE is working!";
    }
    
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
