#include "Ball.h"

Ball::Ball()
{
	//init vars
	m_radius = 30;
	m_pos = ofVec2f(640.0f, 380.0f);
	m_origPos = m_pos;
	m_ballImg.load("ball.png");
	m_hitPlayer.load("Hit.mp3");
	
	//randomize initial movement directions
	if (ofRandom(10) < 5)
		m_Xdir = -1;
	else m_Xdir = 1;
	if (ofRandom(10) < 5)
		m_Ydir = 1;
	else m_Ydir = -1;

	m_speedX = 0;
	m_speedY = 0;
}

Ball::~Ball()
{
}

void Ball::update(ofRectangle box)
{
	if (m_pos.x > box.x + box.width - m_radius)
	{
		//m_pos.x = ofGetWidth() - m_radius;
		m_Xdir *= -1;
	}
	else if (m_pos.x < m_radius + box.x)
	{
		//m_pos.x = m_radius;
		m_Xdir *= -1;
	}
	if (m_pos.y > box.y + box.height - m_radius)
	{
		//m_pos.y = ofGetHeight() - m_radius;
		m_Ydir *= -1;
	}
	else if (m_pos.y < box.y + m_radius)
	{
		//m_pos.y = m_radius;
		m_Ydir *= -1;
	}

	m_pos.x = m_pos.x + (m_speedX * m_Xdir);
	m_pos.y = m_pos.y + (m_speedY * m_Ydir);
}

void Ball::draw()
{
	ofSetRectMode(OF_RECTMODE_CENTER);
	m_ballImg.draw(m_pos.x, m_pos.y);
	ofSetRectMode(OF_RECTMODE_CORNER);
}

//check for player collision with the ball and change the velocity based on the hit angle
void Ball::checkPlayerCollision(ofVec2f player, int rad)
{
	ofVec2f distVec = ofVec2f(m_pos - player);
	float dist = ofDist(m_pos.x, m_pos.y, player.x, player.y);
	float minDist = m_radius + rad;

	if (dist < minDist)
	{
		float dx = m_pos.x - player.x;
		float dy = m_pos.y - player.y;
		float angle = atan2(dy, dx);
		float cosine = cos(angle);
		float sine = sin(angle);


		m_pos.x += cosine * 10.0f;
		m_pos.y += sine * 10.0f;
		m_speedX = cosine * (7 + int(ofRandom(0, 10))) * m_Xdir;
		m_speedY = sine * (7 + int(ofRandom(0, 10))) * m_Ydir;

		m_hitPlayer.play();
	}

}

void Ball::setZeroSpeed()
{
	m_speedX = 0.0f;
	m_speedY = 0.0f;
}

void Ball::reset()
{
	setZeroSpeed();
	m_pos = m_origPos;
}
