#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include "Config.h"
#include "UI_Element.h"
#include "Ball.h"
#include "Player.h"

using namespace std;


class ofApp : public ofBaseApp{

	public:

		//video 
		bool m_isLiveVideo;
		ofVideoGrabber m_videoGrabber;
		ofVideoPlayer m_videoPlayer;

		//sound
		ofSoundPlayer m_bgMusicPlayer;
		ofSoundPlayer m_goalPlayer;
		ofSoundPlayer m_bonkPlayer;
		ofSoundPlayer m_gameOverPlayer;

		//openCV variables
		
		//color-tracking
		ofxCvContourFinder m_contourFinder; //contour finder for player 1
		ofxCvContourFinder m_contourFinder2; //contour finder for player 2
		ofxCvColorImage m_origImage; //the original image
		ofxCvGrayscaleImage m_diffImage; //difference image
		ofxCvGrayscaleImage m_maskedImage1; //image mask for each player/contour
		ofxCvGrayscaleImage m_maskedImage2;

		//background subtraction
		ofxCvGrayscaleImage m_grayscaleImage; //the original image in grayscale
		ofxCvGrayscaleImage m_grayscaleBg; //the background
		ofxCvGrayscaleImage m_grayscaleDiff; //the difference between the background and the current image using pixel-by-pixel subtraction
		ofxCvColorImage m_resizeColImage; //image for resizing the video source into smaller size

		//ofxImGui GUI
		ofxImGui::Gui m_gui;
		ImFont *m_imFont;
		int m_threshold;
		int m_numContoursConsidered;
		float m_minArea;
		float m_maxArea;
		
		//object tracking variables
		bool m_isColorTracking;
		bool m_isBgSubtracting;
		bool m_captureBg;
		
		//tracking color vars
		ofImage m_screenshotImg;
		ofColor m_hoverColor;
		bool m_isPickingColor1;
		bool m_isPickingColor2;
		float m_trackedColor_1[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float m_trackedColor_2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//boolean for game states
		bool m_isDebugging;
		bool m_isTitle;
		bool m_isInstructions;
		bool m_isSetup;
		bool m_isPlaying;
		bool m_gameOver;

		//Game vars
		Player m_player1;
		Player m_player2;
		Ball m_ball;
		ofVec2f m_crashPos;
		ofRectangle m_goal1;
		ofRectangle m_goal2;

		bool m_player1Detected;
		bool m_player2Detected;
		bool m_player1Lost;
		bool m_player2Lost;
		bool m_playersClose;
		bool m_gameOverSfxPlayed;

		int m_player1Score;
		int m_player2Score;
		int m_time;
		int m_startTime;

		float m_widthRatio;
		float m_heightRatio;

		//UI vars

		ofImage m_titleImg;
		ofImage m_instructionsImg;
		ofImage m_setupImg;
		ofImage m_debugImg;
		ofImage m_gameScreenImg;
		ofImage m_gameOverImg;
		ofImage m_restartBoxImg;
		ofImage m_crashImg;
		ofTrueTypeFont m_fontLg;
		ofTrueTypeFont m_fontMd;
		ofTrueTypeFont m_fontSm;
		ofTrueTypeFont m_clockFont;

		UI_Element m_startButton;
		UI_Element m_instructionsButton;
		UI_Element m_setupButton;
		UI_Element m_instructionsBackButton;
		UI_Element m_setupBackButton;
		UI_Element m_restartButton;
		UI_Element m_resumeGameButton;
		ofRectangle m_arena;


		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);
		
		//functions
		void processColor();
		void processBgSubtract();
		void restartGame();
};
