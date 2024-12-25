#pragma once

#include "ofMain.h"

namespace Config
{
	static const int APP_WINDOW_WIDTH = 1280;
	static const int APP_WINDOW_HEIGHT = 720;
	static const int APP_DESIRED_FRAMERATE = 60;

	static const std::string VIDEO_PATH_COLOR = "demo3.mp4";
	static const int VIDEO_WIDTH = 320;		
	static const int VIDEO_HEIGHT = 240;

	static const int VIDEO_BORDER_SIZE = 10;
	static const int SETUP_TOP_MARGIN = 130;
	static const int SETUP_LEFT_MARGIN = 50;

	static const ofColor IDLE_DARK_COL = ofColor(42, 76, 116);
	static const ofColor HOVER_DARK_COL = ofColor(66, 150, 250);
	static const ofColor PRESS_DARK_COL = ofColor(15, 125, 250);
}