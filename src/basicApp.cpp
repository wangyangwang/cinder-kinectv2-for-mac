#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "NiTE.h"

#define MAX_USERS 10
#define MAX_DEPTH 10000

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace openni;
#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

struct user_t {
    bool visible;
    vec2 head;
    vec2 neck;
    vec2 leftShoulder;
    vec2 rightShoulder;
    vec2 leftElbow;
    vec2 rightElbow;
    vec2 leftHand;
    vec2 rightHand;
    vec2 torso;
    vec2 leftHip;
    vec2 rightHip;
    vec2 leftKnee;
    vec2 rightKnee;
    vec2 leftFoot;
    vec2 rightFoot;
    vec3 centerOfMassWorldCoord;
};

class basicApp : public App {
  public:
	void setup() override;
//    void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void calculateHistogram(float* histogram, int histogramSize, const openni::VideoFrameRef& frame);
    vec2 getJointInDepthCoordinates(nite::UserData user, nite::JointType jointType);
    void drawUser(user_t user);
    
    
//    gl::VboMesh depthPixels;
//    gl::Texture depthTexture;
//
    openni::Device device;
    nite::UserTracker userTracker;
    
    float depthHist[MAX_DEPTH];
    user_t users[MAX_USERS];
    
};


void basicApp::setup()
{
    nite::Status niteRc = nite::NiTE::initialize();
    
    if(niteRc!=nite::STATUS_OK){
        logic_error("couldn't start nite");
        return;
    }
    niteRc = userTracker.create();
    
    if(niteRc != nite::STATUS_OK){
        logic_error("couldn't create user tracker!");
        return;
    }
    
    
}

void basicApp::update()
{
    nite::UserTrackerFrameRef userTrackerFrame;
    
    //get raw depth frame
    nite::Status niteRc = userTracker.readFrame(&userTrackerFrame);
    
    //get user labels map
    const nite::UserMap &userLabels = userTrackerFrame.getUserMap();
    const nite::UserId *pixelLabels = userLabels.getPixels();
    
    //process users
    const nite::Array<nite::UserData>& usersData = userTrackerFrame.getUsers();
    
    for(int i=0;i<usersData.getSize();++i){
        const nite::UserData &user = usersData[i];
        int id = user.getId();
        
        if(user.isNew()){
            userTracker.startSkeletonTracking(id);
        }else if(user.getSkeleton().getState() == nite::SKELETON_TRACKED && id < MAX_USERS - 1 ){
            users[id].visible       = user.isVisible();
            
            users[id].head          = getJointInDepthCoordinates(user, nite::JOINT_HEAD);
            users[id].neck          = getJointInDepthCoordinates(user, nite::JOINT_NECK);
            users[id].leftShoulder  = getJointInDepthCoordinates(user, nite::JOINT_LEFT_SHOULDER);
            users[id].rightShoulder = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_SHOULDER);
            users[id].leftElbow     = getJointInDepthCoordinates(user, nite::JOINT_LEFT_ELBOW);
            users[id].rightElbow    = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_ELBOW);
            users[id].leftHand      = getJointInDepthCoordinates(user, nite::JOINT_LEFT_HAND);
            users[id].rightHand     = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_HAND);
            users[id].torso         = getJointInDepthCoordinates(user, nite::JOINT_TORSO);
            users[id].leftHip       = getJointInDepthCoordinates(user, nite::JOINT_LEFT_HIP);
            users[id].rightHip      = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_HIP);
            users[id].leftKnee      = getJointInDepthCoordinates(user, nite::JOINT_LEFT_KNEE);
            users[id].rightKnee     = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_KNEE);
            users[id].leftFoot      = getJointInDepthCoordinates(user, nite::JOINT_LEFT_FOOT);
            users[id].rightFoot     = getJointInDepthCoordinates(user, nite::JOINT_RIGHT_FOOT);
        }
        
        //depth map stuff
//        openni::VideoFrameRef frame = userTrackerFrame.getDepthFrame();
//        calculateHistogram(depthHist, MAX_DEPTH, frame);
//
//        const openni::DepthPixel *depthRow = (const openni::DepthPixel*)frame.getData();
//
//        int resX = frame.getVideoMode().getResolutionX();
//        int resY = frame.getVideoMode().getResolutionY();
//
        
    }
    
    
}

vec2 basicApp::getJointInDepthCoordinates(nite::UserData user, nite::JointType jointType){
    const nite::SkeletonJoint& joint = user.getSkeleton().getJoint(jointType);
    float x, y;
    
    userTracker.convertJointCoordinatesToDepth(joint.getPosition().x, joint.getPosition().y, joint.getPosition().z, &x, &y);
    
    return vec2(x, y);
}

void basicApp::drawUser(user_t user) {
    float r = 3;
    
    gl::drawSolidCircle(user.head,          r);
    gl::drawSolidCircle(user.neck,          r);
    gl::drawSolidCircle(user.leftShoulder,  r);
    gl::drawSolidCircle(user.rightShoulder, r);
    gl::drawSolidCircle(user.leftElbow,     r);
    gl::drawSolidCircle(user.rightElbow,    r);
    gl::drawSolidCircle(user.leftHand,      r);
    gl::drawSolidCircle(user.rightHand,     r);
    gl::drawSolidCircle(user.torso,         r);
    gl::drawSolidCircle(user.leftHip,       r);
    gl::drawSolidCircle(user.rightHip,      r);
    gl::drawSolidCircle(user.leftKnee,      r);
    gl::drawSolidCircle(user.rightKnee,     r);
    gl::drawSolidCircle(user.leftFoot,      r);
    gl::drawSolidCircle(user.rightFoot,     r);
    

    gl::drawLine(user.head,            user.neck);
    gl::drawLine(user.leftShoulder,    user.rightShoulder);
    gl::drawLine(user.leftShoulder,    user.torso);
    gl::drawLine(user.rightShoulder,   user.torso);
    
    gl::drawLine(user.leftShoulder,    user.leftElbow);
    gl::drawLine(user.leftElbow,       user.leftHand);
    
    gl::drawLine(user.rightShoulder,   user.rightElbow);
    gl::drawLine(user.rightElbow,      user.rightHand);
    
    gl::drawLine(user.torso,           user.leftHip);
    gl::drawLine(user.torso,           user.rightHip);
    
    gl::drawLine(user.leftHip,         user.leftKnee);
    gl::drawLine(user.leftKnee,        user.leftFoot);
    
    gl::drawLine(user.rightHip,        user.rightKnee);
    gl::drawLine(user.rightKnee,       user.rightFoot);
}

void basicApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::color(255, 0, 0);
    for(int i=0;i<MAX_USERS;++i){
        if(users[i].visible){
            drawUser(users[i]);
        }
    }
}

CINDER_APP( basicApp, RendererGl )
