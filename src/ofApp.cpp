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
        combinedMesh.drawWireframe();
    }
    if (drawMesh) {
        combinedMesh.draw();
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
    vector<glm::vec3> vertexVector = trace.getVertices();
    newBranchIndex = vertexVector.size();
    
    glm::vec4 startPoint;
    glm::vec4 endPoint;
    glm::quat startOrientation;
    glm::vec3 startDirection;
    // This function should only start after there are at least 3 vertices.
    // Vertex 0 is not included in the polyline visualisation.
    // vertex 1 -> 2 construct first part of the branch.
    for (int i = lastBranchIndex; i < newBranchIndex; i++) {
        if (i == 2) {
            startPoint = glm::vec4(vertexVector[i-1],1.0);
            endPoint = glm::vec4(vertexVector[i],1.0);
            startDirection = endPoint - startPoint;
            
            glm::vec3 endDirection = calculateEndDirection(startPoint, endPoint);
            glm::quat endOrientation = calculateEndOrientation(startDirection, endDirection);
            
            startOrientation = endOrientation;
            
            shared_ptr<ofxBranch> branch(new ofxBranch(startPoint,
                                                       endPoint,
                                                       startOrientation,
                                                       startDirection));
            branches.push_back(branch);
            ofxBranchCylinder::putIntoMesh(branch, this->combinedMesh);
            
        } else if (i > 2) {
            startPoint = branches.back()->getEndPos();
            endPoint = glm::vec4(vertexVector[i].x,
                                 vertexVector[i].y,
                                 vertexVector[i].z,
                                 1.0);
            startOrientation = branches.back()->getEndOrientation();
            startDirection = branches.back()->getEndDirection();
            cout << "startOrientation";
            cout << startOrientation << endl;
            cout << "startDirection";
            cout << startDirection << endl;
            
            shared_ptr<ofxBranch> branch(new ofxBranch(startPoint,
                                                       endPoint,
                                                       startOrientation,
                                                       startDirection));
            branches.push_back(branch);
            ofxBranchCylinder::putIntoMesh(branch, this->combinedMesh);
        }
    }
    
    cout << "branch added" << endl;
    lastBranchIndex = newBranchIndex;
}
glm::quat ofApp::calculateEndOrientation(glm::vec3 startDirection, glm::vec3 endDirection){
    glm::quat topRotation = rotationBetweenVectors(startDirection, endDirection);
    return topRotation * glm::quat(0.0,1.0,1.0,1.0);
}

glm::vec3 ofApp::calculateEndDirection(glm::vec4 startPos, glm::vec4 endPos){
    if(startPos == endPos){
        // if startPos and endPos are the same,
        // calculateEndDirection returns NaN, avoid this situation
        return startPos;
    } else {
        return glm::normalize(glm::vec3(endPos - startPos));
    }
}
glm::quat ofApp::rotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
    start = glm::normalize(start);
    dest = glm::normalize(dest);
    
    float cosTheta = dot(start, dest);
    glm::vec3 rotationAxis;
    
    if (cosTheta < -1 + 0.001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
        
        rotationAxis = glm::normalize(rotationAxis);
        //return gtx::quaternion::angleAxis(180.0f, rotationAxis);
        glm::angleAxis(180.0f, rotationAxis);
    }
    
    rotationAxis = glm::cross(start, dest);
    float s = glm::sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;
    
    return glm::quat(
                     s * 0.5f,
                     rotationAxis.x * invs,
                     rotationAxis.y * invs,
                     rotationAxis.z * invs
                     );
    
}
