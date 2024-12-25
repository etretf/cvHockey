#include "UI_Element.h"

//enable interactions with the UI element
void UI_Element::m_ui_setup()
{
	enableMouseEvents();
	enableKeyEvents();
	m_pressed = false;
	m_released = false;
}

//overwrite the default ofRectangle set function to accept x pos, y pos, width, height, and radius for rounded buttons
void UI_Element::set(float px, float py, float w, float h, float radius)
{
	x = px;
	y = py;
	width = w;
	height = h;
	m_radius = radius;

}


//draw the button according to the interactive states (idle, hover, pressed)
void UI_Element::m_ui_draw()
{
	if (isMousePressed())
	{
		ofSetColor(Config::PRESS_DARK_COL);
	}
	else if (isMouseOver())
	{
		ofSetColor(Config::HOVER_DARK_COL);
	}
	else ofSetColor(Config::IDLE_DARK_COL);

	ofDrawRectRounded(x, y, width, height, m_radius);
	ofSetColor(255);
}



