#pragma once
#include "ofMain.h"

class Ball
{
public:
	int m_radius;
	ofVec2f m_pos;
	ofImage m_ballImg;
	float m_speedX;
	float m_speedY;
	ofVec2f m_origPos;
	ofSoundPlayer m_hitPlayer;

	int m_Xdir;
	int m_Ydir;

	Ball();
	~Ball();

	void update(ofRectangle box);
	void draw();
	void checkPlayerCollision(ofVec2f player, int rad);
	void setZeroSpeed();
	void reset();
};