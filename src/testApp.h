#pragma once
#include "stdio.h"
#import "Carbon/Carbon.h"
#include "ofMain.h"
#include "MSAFluid.h"
#include "ParticleSystem.h"
//=========================================================================

//#define USE_MULTITOUCH
#define USE_OPTIFLOW
#define USE_TUIO // you will need ofxTUIO & ofxOsc
#define USE_GUI // you will need ofxSimpleGuiToo, ofxMSAInteractiveObject & ofxXmlSettings
//#define USE_PSEYE
//#define USE_VIDGRABBER
//#define USE_DRAWNETIC

#if __MAC_OS_X_VERSION_MAX_ALLOWED > 1060
    #undef USE_PSEYE
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    #undef USE_VIDGRABBER
#endif

//=========================================================================

#ifdef USE_DRAWNETIC
    #include "ofxDrawnetic.h"
#endif

#ifdef USE_PSEYE
    #include "ofxMacamPs3Eye.h"
#endif

#ifdef USE_MULTITOUCH
    #include "ofxMultiTouchPad.h"
#endif

#ifdef USE_OPTIFLOW
    #include "ofxOpticalFlowLK.h"
#endif

#ifdef USE_TUIO
    #include "ofxTuio.h"
    #define tuioCursorSpeedMult				0.5	// the iphone screen is so small, easy to rack up huge velocities! need to scale down
    #define tuioStationaryForce				0.001f	// force exerted when cursor is stationary
#endif

#ifdef USE_GUI
    #include "ofxSimpleGuiToo.h"
#endif

class testApp : public ofBaseApp
{
    // OPENFRAMEWORKS
    void setup();
    void update();
    void draw();

    
    // UTIL
    string* vecToArray(vector<string> stringVec);
    void fadeToColor(float r, float g, float b, float speed);

    
    // FLUID
    void setupFluid();
    void updateFluid();
    void addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce);

    msa::fluid::Solver      fluidSolver;
    msa::fluid::DrawerGl	fluidDrawer;
    ParticleSystem          particleSystem;
    
    float                   colorMult;
    float                   velocityMult;
    
    int                     fluidCellsX;
    int                     colorMode;
    
    bool                    resizeFluid;
    bool                    drawFluid;
    bool                    drawParticles;
    bool                    isFrozen;
    bool                    firstTime;

    
    // UI
    #ifdef USE_GUI
        void setupGui();
        void toggleGui();
        void loadSettings(string pageName);
        void setColorModes(int colorMode);
        
        void keyPressed(int key);
        void mouseMoved(int x, int y);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
    
        bool    guiHidden;
        bool    cursorHidden;
        bool    mouseDown;
        ofVec2f lastMouse;
    #endif
    
    
    // TUIO
    #ifdef USE_TUIO
       void updateTUIO();
        ofxTuioClient           tuioClient;
    #endif
    
    
    // PSEYE
    #ifdef USE_PSEYE
        vector<ofxMacamPs3Eye*> cameras;
    #endif
    
    
    // VIDGRABBER
    #ifdef USE_VIDGRABBER
        ofVideoGrabber          vidGrabber;
    #endif
    
    
    // OPTIFLOW
    #ifdef USE_OPTIFLOW
        void setupOptiflow();

        ofxOpticalFlowLK        optiFlow;
        int                     camWidth;
        int                     camHeight;
        float                   threshold;
        float                   maxForce;
        int                     step;
        int                     stepSquared;
        ofPoint                 avgForceTmp;
        bool showOpticalFlow;
    #endif
    
    
    // DRAWNETIC
    #ifdef USE_DRAWNETIC
        ofxMarking *marking;
        ofxSuperPath *path;
        ofxPaint *paint;
        ofxFlockingBrush *brush;
    
        vector<ofxMarking *>::iterator it;
        vector<ofxMarking *> markings;
    #endif
    
    
    // MULTITOUCH
    #ifdef USE_MULTITOUCH
        ofxMultiTouchPad pad;
        std::vector<ofPoint> touches;
    #endif

};