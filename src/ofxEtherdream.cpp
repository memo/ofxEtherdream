#include "ofxEtherdream.h"

//--------------------------------------------------------------
void ofxEtherdream::setup(bool bStartThread) {
    etherdream_lib_start();
    
    setPPS(30000);
    setWaitBeforeSend(false);
    
	/* Sleep for a bit over a second, to ensure that we see broadcasts
	 * from all available DACs. */
	usleep(1000000);
    init();
    
    if(bStartThread) start();
}


//--------------------------------------------------------------
bool ofxEtherdream::stateIsFound() {
    return state == ETHERDREAM_FOUND;
}

//--------------------------------------------------------------
bool ofxEtherdream::checkConnection(bool bForceReconnect) {
    if(device->state == ST_SHUTDOWN) {
        if(bForceReconnect) {
            kill();
            setup();
        }
        return false;
    }
    return true;
}

//--------------------------------------------------------------
void ofxEtherdream::init() {
    int device_num = etherdream_dac_count();
	if (!device_num) {
		ofLogWarning() << "ofxEtherdream::init - No DACs found";
		return 0;
	}
    
	for (int i=0; i<device_num; i++) {
		ofLogNotice() << "ofxEtherdream::init - " << i << " Ether Dream " << etherdream_get_id(etherdream_get(i));
	}
    
	device = etherdream_get(0);
    
	ofLogNotice() << "ofxEtherdream::init - Connecting...";
	if (etherdream_connect(device) < 0) return 1;
    
    ofLogNotice() << "ofxEtherdream::init - done";
    
    state = ETHERDREAM_FOUND;
}

//--------------------------------------------------------------
void ofxEtherdream::threadedFunction() {
    while (isThreadRunning() != 0) {
        
        switch (state) {
            case ETHERDREAM_NOTFOUND:
                if(bAutoConnect) init();
                break;
                
            case ETHERDREAM_FOUND:
                if(lock()) {
                    send();
                    unlock();
                }
                break;
        }
    }
}

//--------------------------------------------------------------
void ofxEtherdream::start() {
    startThread(true, false);  // TODO: blocking or nonblocking?
}

//--------------------------------------------------------------
void ofxEtherdream::stop() {
    stopThread();
}

//--------------------------------------------------------------
void ofxEtherdream::send() {
    if(!stateIsFound() || points.empty()) return;
    
    if(bWaitBeforeSend) etherdream_wait_for_ready(device);
    else if(!etherdream_is_ready(device)) return;
    
    // DODGY HACK: casting ofxIlda::Point* to etherdream_point*
    int res = etherdream_write(device, (etherdream_point*)points.data(), points.size(), pps, 1);
    if (res != 0) {
        ofLogVerbose() << "ofxEtherdream::write " << res;
    }
    points.clear();
}


//--------------------------------------------------------------
void ofxEtherdream::clear() {
    if(lock()) {
        points.clear();
        unlock();
    }
}

//--------------------------------------------------------------
void ofxEtherdream::addPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        if(!_points.empty()) {
            points.insert(points.end(), _points.begin(), _points.end());
        }
        unlock();
    }
}


//--------------------------------------------------------------
void ofxEtherdream::addPoints(const ofxIlda::Frame &ildaFrame) {
    addPoints(ildaFrame.getPoints());
}


//--------------------------------------------------------------
void ofxEtherdream::setPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        points = _points;
        unlock();
    }
}


//--------------------------------------------------------------
void ofxEtherdream::setPoints(const ofxIlda::Frame &ildaFrame) {
    setPoints(ildaFrame.getPoints());
}

//--------------------------------------------------------------
void ofxEtherdream::setWaitBeforeSend(bool b) {
    if(lock()) {
        bWaitBeforeSend = b;
        unlock();
    }
}

//--------------------------------------------------------------
bool ofxEtherdream::getWaitBeforeSend() const {
    return bWaitBeforeSend;
}


//--------------------------------------------------------------
void ofxEtherdream::setPPS(int i) {
    if(lock()) {
        pps = i;
        unlock();
    }
}

//--------------------------------------------------------------
int ofxEtherdream::getPPS() const {
    return pps;
}
