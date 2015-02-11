//
//  ofxEtherdream.h
//  ofxILDA
//
//  Created by Daito Manabe + Yusuke Tomoto (Rhizomatiks)
//  Mods by Memo Akten
//
//


#include "ofMain.h"
#include "etherdream.h"
#include "ofxIldaFrame.h"

class ofxEtherdream : public ofThread {
public:
    ofxEtherdream():state(ETHERDREAM_NOTFOUND), bAutoConnect(false) {}
    
    ~ofxEtherdream() {
        kill();
    }
    
    bool stateIsFound();
    
    void kill() {
        clear();
        stop();
        if(stateIsFound()) {
            etherdream_stop(device);
            etherdream_disconnect(device);
        }
    }
    
    void setup(bool bStartThread = true, int idEtherdream = 0);
    virtual void threadedFunction();
    
    
    // check if the device has shutdown (weird bug in etherdream driver) and reconnect if nessecary
    bool checkConnection(bool bForceReconnect = true);
    
    void clear();
    void start();
    void stop();

    void addPoints(const vector<ofxIlda::Point>& _points);
    void addPoints(const ofxIlda::Frame &ildaFrame);
    
    void setPoints(const vector<ofxIlda::Point>& _points);
    void setPoints(const ofxIlda::Frame &ildaFrame);
    
    void send();
    
    void setPPS(int i);
    int getPPS() const;
    
    void setWaitBeforeSend(bool b);
    bool getWaitBeforeSend() const;
    
private:
    void init();
    
private:
    enum {
        ETHERDREAM_NOTFOUND = 0,
        ETHERDREAM_FOUND
    } state;
    
    int pps;
    bool bWaitBeforeSend;
    bool bAutoConnect;
    
    struct etherdream *device;
    vector<ofxIlda::Point> points;
    
    int idEtherdreamConnection;
};