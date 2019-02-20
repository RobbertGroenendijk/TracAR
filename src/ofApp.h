#pragma once


// OpenFrameworks
#include "ofMain.h"
// ofxAddons
#include "ofxBranchesPrimitive.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"

#define PORT 12001
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        // Personal functions
        void constructDataPoint(float _xPos, float _yPos, float _zPos);
        void constructVertexPoint(float _xPos, float _yPos, float _zPos);
        void constructPolyline(float _xPos, float _yPos, float _zPos);
        void constructBranch();
    
    // OSC
    ofxOscReceiver OSCreceiver;
    
    // 3D environment
    ofEasyCam virtualCamera;
    ofMaterial mat;
    ofLight light;
    
    // Branches
    ofMesh combinedMesh;
    mutable ofMesh normalsMesh;
    vector<shared_ptr<ofxBranch>> branches;
    
    vector<ofSpherePrimitive> dataPoints;
    vector<ofSpherePrimitive> vertexPoints;
    
    // Variables
    bool drawWireframe, drawMesh, drawDataPoints, drawVertexPoints;
    glm::vec4 firstStartPoint;
    glm::vec4 firstStartDirection;
    glm::vec3 incomingPosition;
    glm::vec3 lastIncomingPosition;
    
    // Polyline for initial tracing
    ofPolyline trace;
    
    // Index for conversion polyline -> branch -> mesh
    int lastIndex;
    int newIndex;
    int lastBranchIndex;
    int newBranchIndex;
    
    // Tube
    ofxBranchesPrimitive tube;
};
