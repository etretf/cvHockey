#include "Player.h"

Player::Player()
{
	m_pos = ofVec2f(0, 0);
	m_radius = 50.0f;
	m_imgRadius = 50.0f;
}

Player::~Player()
{
}

void Player::draw()
{
	ofSetCircleResolution(100);
	if (m_playerNum == 1)
	{	
		ofSetColor(255, 133, 133);
		ofDrawCircle(m_pos, m_radius);
		ofSetColor(153, 3, 3);
		ofDrawCircle(m_pos, m_radius * 0.85f);
		ofSetColor(255, 133, 133);
		ofDrawCircle(m_pos, m_radius * 0.5f);
		ofSetColor(153, 3, 3);
		ofDrawCircle(m_pos, m_radius * 0.35f);
	}
	if (m_playerNum == 2)
	{
		ofSetColor(238, 133, 255);
		ofDrawCircle(m_pos, m_radius);
		ofSetColor(99, 33, 131);
		ofDrawCircle(m_pos, m_radius * 0.85f);
		ofSetColor(238, 133, 255);
		ofDrawCircle(m_pos, m_radius * 0.5f);
		ofSetColor(99, 33, 131);
		ofDrawCircle(m_pos, m_radius * 0.35f);
	}
	ofSetColor(255);

}
