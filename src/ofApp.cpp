#include "ofApp.h"

#include <stdio.h>
#include <iostream>
#include <string.h>

//--------------------------------------------------------------
void ofApp::setup(){
    // 3D environment
    light.setup();
    light.setAmbientColor(ofFloatColor::yellow);
    light.setPosition(-50, 200, 0);
    light.enable();
    mat.setDiffuseColor(ofFloatColor::purple);
    mat.setEmissiveColor(ofFloatColor::blue);
    
    // OSC
    std::cout << "listening for osc messages on port " << PORT << "\n";
    OSCreceiver.setup(PORT);
    
    // Variables
    drawWireframe = true;
    drawMesh = false;
    drawDataPoints = false;
    drawVertexPoints = true;
    
    firstStartPoint = glm::vec4(0.0f,0.0f,0.0f,0.0f);
    firstStartDirection = glm::vec4(0.0f,0.0f,0.0f,0.0f);
    
    lastIndex = 0;
    newIndex = 0;
    // Branch should only start constructing after at least 3 vertices in polyline.
    // Vertex 0 is not included. Vertex 1 -> 2 construct the first branch part.
    lastBranchIndex = 1;
    newBranchIndex = 2;
    
    tube.build();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // OSC
    float branchXpos,branchYpos,branchZpos;
    while(OSCreceiver.hasWaitingMessages()) {
        ofxOscMessage OSCmessage;
        OSCreceiver.getNextMessage(&OSCmessage);
        
        for (int i = 0; i < OSCmessage.getNumArgs(); i++) {
            if (OSCmessage.getAddress().compare("/xPosition") == 0) {
                cout << "xPos: ";
                cout << OSCmessage.getArgAsFloat(0) << endl;
                incomingPosition.x = OSCmessage.getArgAsFloat(0);
            } else if (OSCmessage.getAddress().compare("/yPosition") == 0) {
                cout << "yPos: ";
                cout << OSCmessage.getArgAsFloat(0) << endl;;
                incomingPosition.y = OSCmessage.getArgAsFloat(0);
            } else if (OSCmessage.getAddress().compare("/zPosition") == 0) {
                cout << "zPos: ";
                cout << OSCmessage.getArgAsFloat(0) << endl;
                incomingPosition.z = OSCmessage.getArgAsFloat(0);
            }
        }
    }
    
    if (incomingPosition != lastIncomingPosition) {
        constructDataPoint(incomingPosition.x,incomingPosition.y,incomingPosition.z);
        constructPolyline(incomingPosition.x,incomingPosition.y,incomingPosition.z);
        
        lastIncomingPosition = incomingPosition;
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(ofFloatColor::peachPuff);
    ofEnableDepthTest();
    virtualCamera.begin();
    light.draw();
    ofSetColor(0,0,0,100);
    mat.begin();
    
    if (drawDataPoints) {
        for (int i = 0; i < dataPoints.size(); i++) {
            dataPoints[i].draw();
        }
    }
    
    if (drawVertexPoints) {
        for (int i = 0; i < vertexPoints.size(); i++) {
            vertexPoints[i].draw();
        }
    }
    
    
    if (drawWireframe) {
        //combinedMesh.drawWireframe();
        tube.drawWireframe();
    }
    if (drawMesh) {
        //combinedMesh.draw();
        tube.draw();
    }
    
    trace.draw();
    
    mat.end();
    virtualCamera.end();
    ofDisableDepthTest();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'w':
            drawWireframe = !drawWireframe;
            break;
        case 'm':
            drawMesh = !drawMesh;
            break;
        case 'p':
            drawDataPoints = !drawDataPoints;
            break;
        case 'v':
            drawVertexPoints = !drawVertexPoints;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::constructDataPoint(float _xPos, float _yPos, float _zPos) {
    dataPoints.push_back(*new ofSpherePrimitive);
    dataPoints[dataPoints.size()-1].set(10, 10);
    dataPoints[dataPoints.size()-1].setPosition(_xPos, _yPos, _zPos);
}
void ofApp::constructVertexPoint(float _xPos, float _yPos, float _zPos) {
    vertexPoints.push_back(*new ofSpherePrimitive);
    vertexPoints[vertexPoints.size()-1].set(2, 10);
    vertexPoints[vertexPoints.size()-1].setPosition(_xPos, _yPos, _zPos);
}
void ofApp::constructPolyline(float _xPos, float _yPos, float _zPos) {
    if (trace.getVertices().size() == 0) {
        trace.curveTo(ofVec3f(_xPos,_yPos,_zPos));
    } else {
        trace.curveTo(ofVec3f(_xPos,_yPos,_zPos));
    }
    trace.getSmoothed(20);
    
    vector<glm::vec3> vertexVector = trace.getVertices();
    newIndex = vertexVector.size();
    for (int i = lastIndex; i < newIndex; i++) {
        glm::vec3 currentVertex = vertexVector[i];
        constructVertexPoint(currentVertex.x,
                             currentVertex.y,
                             currentVertex.z);
    }
    lastIndex = newIndex;
    
    // Construct branch if we have at least 1 begin & endpoint.
    if (trace.getVertices().size() >= 3) {
        constructBranch();
    }
}
void ofApp::constructBranch() {
    tube.clear();
    for (int i = 0; i < vertexPoints.size(); i++) {
        tube.addVertex(glm::vec4(vertexPoints[i].getPosition().x,
                                 vertexPoints[i].getPosition().y,
                                 vertexPoints[i].getPosition().z,1.0));
    }
    tube.build();
}
