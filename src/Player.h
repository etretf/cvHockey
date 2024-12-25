#pragma once
#include "ofxMSAInteractiveObject.h"
#include "Config.h"
#include "ofMain.h"

class Player
{
public:
	ofVec2f m_pos;
	ofRectangle m_box;
	float m_radius;
	float m_imgRadius;
	int m_playerNum;

	Player();
	~Player();

	void draw();
};