#import "testApp.h"

void testApp::setup()
{
    ofSetFrameRate(120);
    ofSetVerticalSync(false);
    ofEnableAlphaBlending();
	ofBackground(0, 0, 0, 255);
	ofSetBackgroundAuto(false);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
//    firstTime = true;
    guiHidden = TRUE;
    gui.hide();

    cursorHidden = TRUE;
    ofHideCursor();

    setupFluid();
    
    // Setup camera and optical flow engine
    camWidth 		= 320;	// try to grab at this size.
	camHeight 		= 240;
    
    
    #ifdef USE_DRAWNETIC
        path = new ofxSuperPath();
        path->reset();

        marking = NULL;
        brush = NULL;
        paint = NULL;
    #endif
    
    
    #ifdef USE_OPTIFLOW
        this->setupOptiflow();
    #endif

    #ifdef USE_PSEYE
        vector<ofxMacamPs3EyeDeviceInfo*> deviceList = ofxMacamPs3Eye::getDeviceList();

        for (int i = 0; i < deviceList.size(); i++) {
            ofxMacamPs3Eye * camera = new ofxMacamPs3Eye();
            camera->setDeviceID(deviceList[i]->id);
            camera->setDesiredFrameRate(180);
            camera->initGrabber(320, 240, false);
            cameras.push_back(camera);
        }
    #endif
    
    #ifdef USE_VIDGRABBER
        vidGrabber.initGrabber(camWidth,camHeight);
    #endif
        
    #ifdef USE_TUIO
        tuioClient.start(3333);
    #endif
        
    #ifdef USE_GUI
        this->setupGui();
    #endif
        
    #ifdef USE_MULTITOUCH
        pad = ofxMultiTouchPad();
    #endif
    
}

void testApp::setupOptiflow()
{
    threshold = 5;
    maxForce = 40.0f;
    avgForceTmp;
    
    step = 10;
    showOpticalFlow = true;
    optiFlow.bMirrorH = FALSE;
    optiFlow.setup(camWidth,camHeight);
    
    //    optiFlow.setOpticalFlowBlur(10);
    //    optiFlow.setOpticalFlowSize(10);
}

void testApp::setupFluid()
{
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true)
    .setFadeSpeed(0.002)
    .setDeltaT(0.5)
    .setVisc(0.00015)
    .setColorDiffusion(0);
    
	fluidDrawer.enableAlpha(true);
    fluidDrawer.setup(&fluidSolver);
	fluidCellsX			= 150;
	drawFluid			= true;
	drawParticles		= true;
    resizeFluid			= true;

    this->setColorModes(0);
}

#ifdef USE_GUI
void testApp::setupGui()
{
    // Page 1
    gui.addButton("resizeFluid", resizeFluid);
    
    string* titles = this->vecToArray(msa::fluid::getDrawModeTitles());
    gui.addComboBox("fd.drawMode",              (int&)fluidDrawer.drawMode, 4 , titles);
    
    gui.addSlider("fluidCellsX", fluidCellsX,                           20, 400);
    gui.addSlider("colorMult", colorMult,                               0, 100);
    gui.addSlider("velocityMult", velocityMult,                         0, 100);
    
    gui.addSlider("fs.viscocity", fluidSolver.viscocity,                0.0, 0.01);
    gui.addSlider("fs.colorDiffusion", fluidSolver.colorDiffusion,      0.0, 0.0003);
    gui.addSlider("fs.fadeSpeed", fluidSolver.fadeSpeed,                0.0, 0.1);
    gui.addSlider("fs.solverIterations", fluidSolver.solverIterations,  1, 50);
    gui.addSlider("fs.deltaT", fluidSolver.deltaT,                      0.1, 5);

    gui.addToggle("fs.doRGB",                                           fluidSolver.doRGB);
    gui.addToggle("fs.doVorticityConfinement",                          fluidSolver.doVorticityConfinement);
    gui.addToggle("drawFluid",                                          drawFluid);
    gui.addToggle("drawParticles",                                      drawParticles);
    gui.addToggle("fs.wrapX",                                           fluidSolver.wrap_x);
    gui.addToggle("fs.wrapY",                                           fluidSolver.wrap_y);
    
    // Page 2
    gui.addPage();

#ifdef USE_OPTIFLOW
    gui.addSlider("of.threshold", threshold,                            0, 50.0);
    gui.addSlider("of.maxforce", maxForce,                              0, 40.0);
    gui.addSlider("of.step", step,                              1, 100);
    
    gui.addSlider("of.blur", optiFlow.opticalFlowBlur, 0, 100);
    gui.addSlider("of.min", optiFlow.opticalFlowMin, 0, 100);
    gui.addSlider("of.max", optiFlow.opticalFlowMax, 0, 100);
    
    gui.addToggle("of.show", showOpticalFlow);
    gui.addToggle("of.mirrorH", optiFlow.bMirrorH);
    gui.addToggle("of.mirrorV", optiFlow.bMirrorV);
#endif
    
    
    gui.setDefaultKeys(true);
    this->loadSettings("ofxMSAFluidSettings");
//    gui.page(0).setXMLName("ofxMSAFluidSettings1.xml");
//    gui.page(1).setXMLName("ofxMSAFluidSettings2.xml");
    
    gui.loadFromXML();
    gui.setAutoSave(false);

    guiHidden ? gui.hide() : gui.show();
}
#endif

void testApp::mouseDragged(int x, int y, int button)
{
    #ifdef USE_DRAWNETIC
        path->lineTo(x,y,0, ofFloatColor(0), ofGetFrameNum(), 0);
    #endif
}


void testApp::mouseReleased(int x, int y, int button)
{
    #ifdef USE_DRAWNETIC
        path->lineEnd(x,y,0, ofGetFrameNum(), -1, 0);
        marking->pathFinished();
        markings.push_back(marking);
        marking = NULL;
    #endif
}

void testApp::mousePressed(int x, int y, int button)
{
    mouseDown = ofGetMousePressed();
    
#ifdef USE_DRAWNETIC
    path = new ofxSuperPath();
    paint = new ofxPaint(path, ofColor(255), 100);
    brush = new ofxFlockingBrush(path, paint);
    brush->setDynamic(true);
    brush->setSpeed(10);
    brush->setJitter(3.5);
    brush->setRefineShape(false);
    
    
    marking = new ofxMarking(path, paint, brush);
    path->reset();
    path->lineStart(x,y,0, ofFloatColor(0), ofGetFrameNum(), 0);
#endif
}

void testApp::update()
{
    #ifdef  USE_DRAWNETIC

        bool killSome = false;
        if(ofGetFrameRate() < 50)
        {
            killSome = true;
        }
        
        if(marking != NULL)
        {
            marking->update();
        }
        
        for ( it=markings.begin() ; it < markings.end(); it++ )
        {
            ofxMarking *marking = *it;
            if(killSome)
            {
                marking->killLastParticle();
                if(marking->isDead)
                {
                    delete marking;
                    markings.erase(it);
                }
            }
            else
            {
                marking->update();
//                marking->draw()
            }
        }
    #endif
        
    #ifdef USE_MULTITOUCH
        pad.getTouchesAsOfPoints(&touches);
    
        std::vector<MTouch> mTouches = pad.getTouches();
        for (std::vector<MTouch>::iterator touch=mTouches.begin(); touch!=mTouches.end(); ++touch)
        {
            this->addToFluid(ofVec2f(touch->x, touch->y), ofVec2f(0.01*touch->size, 0.01*touch->size), true, true);
        }
    #endif

    #ifdef USE_PSEYE
        for (int i = 0; i < cameras.size(); i++) {
            cameras[i]->update();
        }
        ofxMacamPs3Eye *camera = cameras[0];
    #endif
        
    #ifdef USE_OPTIFLOW
        
        bool hasNewFrame;

        #ifdef USE_PSEYE
            hasNewFrame = camera->isFrameNew();
        #endif
        #ifdef USE_VIDGRABBER
            hasNewFrame = vidGrabber.isFrameNew();
        #endif
        
        if (hasNewFrame)
        {
            #ifdef USE_PSEYE
                optiFlow.update(camera->getPixels(), 320,240,OF_IMAGE_COLOR);
            #endif
            
            #ifdef USE_VIDGRABBER
                optiFlow.update(vidGrabber);
            #endif
            
            stepSquared = step*step;

            for (int x=0; x<camWidth; x+=step) {
                for (int y=0; y<camHeight; y+=step) {
                    
                    avgForceTmp = ofPoint(0,0);

                    for (int xx=x; xx<x+step; xx++) {
                        for (int yy=y; yy<y+step; yy++) {
                            avgForceTmp += optiFlow.getVelAtPixel(xx,yy);
                        }
                    }
                    
                    ofVec2f avgForce = avgForceTmp / stepSquared;
                    
                    if (avgForce.x > threshold || avgForce.x < -threshold || avgForce.y > threshold || -avgForce.y < -threshold)
                    {
                        ofVec2f centerPoint = ofVec2f( x + (step/2), y + (step/2) );
                        this->addToFluid(ofVec2f(centerPoint.x/camWidth, centerPoint.y/camHeight),
                                         ofVec2f(0.01, 0.01), //avgForce.x/maxForce, avgForce.y/maxForce),
                                         TRUE, TRUE);
                    }
                }
            }
        }
    #endif

    if (isFrozen) return;
    
    #ifdef USE_TUIO
        this->updateTUIO();
    #endif
    
#ifdef USE_DRAWNETIC
    for (int i=0; i<markings.size(); i++)
    {
        ofxMarking *marking = markings[i];
        ofxParticleSystem *psys = ((ofxFlockingBrush*)marking->brush)->psys;
        for (int j=0; j<psys->particles.size(); j++)
        {
            ofxParticle *p = psys->particles[j];
            addToFluid(ofVec2f(p->getPos().x /(float)ofGetScreenWidth(), p->getPos().y/(float)ofGetScreenHeight()), ofVec2f(0.01, 0.01), true, true);
        }
        
    }
#endif
//    for(int i = 0; i < markings.size(); i++)
//    {
//        ofxMarking *marking = markings[i];
//        
//        if( ((ofxFlockingBrush*)marking->brush)->faces.size() > 0)
//        {
//            ofxFace *firstface = ((ofxFlockingBrush*)marking->brush)->faces[0];
//            ofVec3f *posPtr = firstface->getPos(0);
//            addToFluid(ofVec2f(posPtr->x/(float)ofGetScreenWidth(), posPtr->y/(float)ofGetScreenHeight()), ofVec2f(0.01, 0.01), true, true);
//        }
//    }
    
    
    this->updateFluid();
}

void testApp::updateFluid()
{
	if(resizeFluid)
    {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / msa::getWindowAspectRatio());
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
        particleSystem = ParticleSystem(colorMode);
	}
    
	fluidSolver.update();
}

#ifdef USE_TUIO
void testApp::updateTUIO()
{
	tuioClient.getMessage();
	
	// do finger stuff (that's what she said)
	list<ofxTuioCursor*>cursorList = tuioClient.getTuioCursors();   
	for(list<ofxTuioCursor*>::iterator it=cursorList.begin(); it != cursorList.end(); it++)
    {
		ofxTuioCursor *tcur = (*it);
        float vx = tcur->getXSpeed() * tuioCursorSpeedMult;
        float vy = tcur->getYSpeed() * tuioCursorSpeedMult;
        
        if(vx == 0 && vy == 0)
        {
            vx = ofRandom(-tuioStationaryForce, tuioStationaryForce);
            vy = ofRandom(-tuioStationaryForce, tuioStationaryForce);
        }
        
        ofVec2f posVec;
        posVec.set(tcur->getX(), tcur->getY());
        
        ofVec2f velVec;
        velVec.set(vx, vy);
        
        this->addToFluid(posVec,velVec,true,true);
    }
}
#endif


void testApp::draw()
{
    ofClear(0,0,0,128);

    if (isFrozen) return;
    
    ofClear(0,0,0,16);
    
	if(drawFluid)
    {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
    else
    {
        this->fadeToColor(0, 0, 0, 0.01);
	}
	
    if(drawParticles)
    {
		particleSystem.updateAndDraw(fluidSolver, ofGetWindowSize(), drawFluid);
	}

    #ifdef USE_GUI
        gui.draw();
    #endif
        
        
    #ifdef USE_OPTIFLOW
        if (showOpticalFlow)
        {
            ofSetColor(255,255,255);
            optiFlow.draw(ofGetWidth(), ofGetHeight(),5,10);
        }
    #endif
    
    #ifdef USE_DRAWNETIC
        if(marking != NULL)
        {
            marking->draw();
        }
        
        for(int i = 0; i < markings.size(); i++)
        {
            markings[i]->draw();
        }
    #endif
    
}


// add force and dye to fluid, and create particles
void testApp::addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce)
{
    float waRatio = msa::getWindowAspectRatio();
    float speed = vel.x * vel.x + vel.y * vel.y * waRatio * waRatio;    // balance x and y components of speed with screen aspect ratio
    
    if(speed > 0)
    {
		pos.x = ofClamp(pos.x, 0.0f, 1.0f);
		pos.y = ofClamp(pos.y, 0.0f, 1.0f);
		
        int index = fluidSolver.getIndexForPos(pos);
        
		if(addColor)
        {
            ofColor drawColor;
            if (fluidSolver.doRGB)
            {
                drawColor.setHsb((ofGetFrameNum() % 255), 255, 255);
                //                drawColor.setHsb(255, 0, 0);
			}
            else
            {
                drawColor.setHsb(255, 255, 255);
            }
            
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
			
			if(drawParticles)
            {
				particleSystem.addParticles(pos * ofVec2f(ofGetWindowSize()), 10);
            }
		}
		
		if(addForce)
        {
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
        }
    }
}

void testApp::keyPressed(int key)
{
    switch (key)
    {
        case 'f': ofToggleFullscreen(); break;
        case 'x': this->toggleGui();    break;

        case 'd': drawFluid     ^= true;    break;
        case 'p': drawParticles ^= true;    break;

        case 'r': resizeFluid    = true;    break;
        case 's':
            for(int i=0; i<gui.getPages().size(); i++) {
                gui.page(i).saveToXML();
            }
            break;

        case 'q': fluidDrawer.setDrawMode(msa::fluid::kDrawColor);  break;
        case 'w': fluidDrawer.setDrawMode(msa::fluid::kDrawMotion); break;
        case 'a': fluidDrawer.setDrawMode(msa::fluid::kDrawSpeed);  break;
        case 'z': fluidDrawer.setDrawMode(msa::fluid::kDrawVectors);break;

        case '0': this->setColorModes(0); break;
        case '1': this->setColorModes(1); break;
        case '2': this->setColorModes(4); break;
        case '3': this->setColorModes(3); break;
        case '4': this->setColorModes(2); break;
        case '5': this->setColorModes(5); break;
        case '6': this->setColorModes(6); break;
        case '7': this->setColorModes(7); break;
        case '8': this->setColorModes(8); break;
        case '9': this->setColorModes(9); break;
        case OF_KEY_RIGHT:            gui.nextPage(); break;
        case OF_KEY_LEFT:             gui.prevPage(); break;

    
    }
//        {
//            case kVK_ANSI_1: printf("loading 1"); [self loadSettings:"ofxMSAFluidSettings multiuser fluid"];  break;
//            case kVK_ANSI_2: printf("loading 2"); [self loadSettings:"ofxMSAFluidSettings lightplay"];        break;
//            case kVK_ANSI_3: printf("loading 3"); [self loadSettings:"ofxMSAFluidSettings nice blue flow"];   break;
//            case kVK_ANSI_4: printf("loading 4"); [self loadSettings:"ofxMSAFluidSettings particles only"];   break;
//            case kVK_ANSI_V:
//                if (fluidSolver.viscocity < 0.0095) {
//                    fluidSolver.viscocity += 0.0005;
//                }
//                break;
//
//        }
//
//        return;
//    }
//
//    
//    // No shift key, change settings
//    switch( [event keyCode] )
//    {
//        case kVK_ANSI_Period:
//            if (fluidCellsX <= 195) {
//                fluidCellsX += 5;
//            }
//            break;
//        case kVK_ANSI_Comma:
//            if (fluidCellsX >= 5) {
//                fluidCellsX -= 5;
//            }
//            break;
//        case kVK_ANSI_V:
//            if (fluidSolver.viscocity >= 0.0005) {
//                fluidSolver.viscocity -= 0.0005;
//            };
//            break;
//        
//		case kVK_ANSI_F: [self toggleFullscreen]; break; //ofToggleFullscreen();     break;
//      case kVK_ANSI_X: [self toggleGui];              break;
//		
//      case kVK_ANSI_D: drawFluid     ^= true;    break;
//		case kVK_ANSI_P: drawParticles ^= true;    break;
//
//		case kVK_ANSI_R: resizeFluid    = true;    break;
//		case kVK_ANSI_S:
//            for(int i=0; i<gui.getPages().size(); i++) {
//                gui.page(i).saveToXML();
//            }
//            break;
//            
//      case kVK_ANSI_Q: fluidDrawer.setDrawMode(msa::fluid::kDrawColor);  break;
//		case kVK_ANSI_W: fluidDrawer.setDrawMode(msa::fluid::kDrawMotion); break;
//		case kVK_ANSI_A: fluidDrawer.setDrawMode(msa::fluid::kDrawSpeed);  break;
//		case kVK_ANSI_Z: fluidDrawer.setDrawMode(msa::fluid::kDrawVectors);break;
//
//		case kVK_ANSI_0: [self setColorModes:0]; break;
//		case kVK_ANSI_1: [self setColorModes:1]; break;
//		case kVK_ANSI_2: [self setColorModes:4]; break;
//		case kVK_ANSI_3: [self setColorModes:3]; break;
//		case kVK_ANSI_4: [self setColorModes:2]; break;
//		case kVK_ANSI_5: [self setColorModes:5]; break;
//		case kVK_ANSI_6: [self setColorModes:6]; break;
//		case kVK_ANSI_7: [self setColorModes:7]; break;
//		case kVK_ANSI_8: [self setColorModes:8]; break;
//		case kVK_ANSI_9: [self setColorModes:9]; break;
//            
////        case kVK_ANSI_V: showOpticalFlow = !showOpticalFlow;
//            
//        case kVK_Space: isFrozen = !isFrozen;   break;
//        
//
//        case kVK_UpArrow:               break;
//        case kVK_DownArrow:             break;
//            
//        case kVK_ANSI_B:
//            {
//                // Timer timer;
//                // const int ITERS = 3000;
//                // timer.start();
//                // for(int i = 0; i < ITERS; ++i) fluidSolver.update();
//                // timer.stop();
//                // cout << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
//            }
//            break;
//    }
}

void testApp::setColorModes(int cMode)
{
    colorMode = cMode;
    fluidDrawer.colorMode = colorMode;
    resizeFluid = true;
    
    switch (colorMode)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            break;
            
    }

    fluidSolver.viscocity = 0.0022;
    
}


void testApp::toggleGui()
{
    guiHidden = !guiHidden;
    guiHidden ? gui.hide() : gui.show();
    cursorHidden = !cursorHidden;
    cursorHidden ? ofHideCursor() : ofShowCursor();
}

//--------------------------------------------------------------
void testApp::loadSettings(string pageName)
{
    for (int i=0; i<gui.getPages().size(); i++)
    {
        char blah[100] = {};
        
        sprintf(blah, "%s%d.xml", pageName.c_str(), i);
        
        gui.page(i).setXMLName(blah);
        gui.page(i).loadFromXML();
        
    }
    resizeFluid = true;
}


void testApp::mouseMoved(int x, int y)
{
    #ifndef USE_MULTITOUCH
        ofVec2f eventPos = ofVec2f(x, y);
        ofVec2f mouseNorm = eventPos / ofGetWindowSize();
        ofVec2f mouseVel = ofVec2f(eventPos - lastMouse) / ofGetWindowSize(); // calculates vector here from last position
        this->addToFluid(mouseNorm,mouseVel,true,true);
        lastMouse = eventPos;
    #endif
}

void testApp::fadeToColor(float r, float g, float b, float speed)
{
    glColor4f(r, g, b, 0.01); //speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}

string* testApp::vecToArray(vector<string> stringVec)
{
    int numItems = stringVec.size();
    string *stringArr= new string[numItems];
    
    for(int i=0; i<numItems; i++)
    {
        stringArr[i] = stringVec[i]; //Copy the vector to the string
    }
    return stringArr;
}