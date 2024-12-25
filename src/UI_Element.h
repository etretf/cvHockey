#pragma once
#include "ofxMSAInteractiveObject.h"
#include "Config.h"
#include "ofMain.h"
class UI_Element : public ofxMSAInteractiveObject
{
public:

	ofVec2f m_ui_pos;
	ofColor m_base_col;
	ofColor m_pressed_col;
	ofColor m_hover_col;
	std::string m_ui_text;
	float m_radius;
	bool m_pressed;
	bool m_released;

	void m_ui_setup();
	void set(float px, float py, float w, float h, float radius);
	void m_ui_draw();
};

