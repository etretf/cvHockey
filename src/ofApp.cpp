//Emma Souannhaphanh
//A3: Visual Interaction
// IMD3005A
#include "ofApp.h"
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(Config::APP_WINDOW_WIDTH, Config::APP_WINDOW_HEIGHT);
	ofSetFrameRate(Config::APP_DESIRED_FRAMERATE);

	//set to true for webcam, false for demo video
	m_isLiveVideo = true;

	//setup webcam
	m_videoGrabber.setup(640, 480);

	//load video file
	m_videoPlayer.load(Config::VIDEO_PATH_COLOR);
	m_videoPlayer.setVolume(0);
	m_videoPlayer.setLoopState(OF_LOOP_NORMAL);
	m_videoPlayer.play();

	//allocate CV images
	m_origImage.allocate(640, 480); //original frame from the video file
	m_grayscaleImage.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_grayscaleBg.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_grayscaleDiff.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_diffImage.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_resizeColImage.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_maskedImage1.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);
	m_maskedImage2.allocate(Config::VIDEO_WIDTH, Config::VIDEO_HEIGHT);

	//init vars
	m_threshold = 80;
	m_numContoursConsidered = 1; //1 per contour finder
	m_minArea = 10.0f;
	m_maxArea = 15000.0f;
	m_captureBg = true;
	m_isBgSubtracting = false;
	m_isColorTracking = true;
	m_isPickingColor1 = false;
	m_isPickingColor2 = false;
	
	//sound players
	m_bgMusicPlayer.load("spaceChillout.mp3");
	m_bgMusicPlayer.setLoop(true);
	m_bgMusicPlayer.play();
	m_goalPlayer.load("Success.mp3");
	m_bonkPlayer.load("bonk.mp3");
	m_gameOverPlayer.load("tada.mp3");
	m_gameOverSfxPlayed = false; //used to play sound only once when game is over

	//game state vars
	m_isDebugging = false;
	m_isTitle = true;
	m_isInstructions = false;
	m_isSetup = false;
	m_isPlaying = false;
	m_gameOver = false;


	//set player num to draw different coloured paddles
	m_player1.m_playerNum = 1;
	m_player2.m_playerNum = 2;

	//detection vars
	m_player1Detected = false;
	m_player2Detected = false;
	m_player1Lost = false;
	m_player2Lost = false;
	m_playersClose = false;

	//init game setting vars
	m_player1Score, m_player2Score = 0;
	m_startTime = 61;
	m_time = m_startTime;
	m_arena.x = 65;
	m_arena.y = 82;
	m_arena.width = 1147;
	m_arena.height = 607;

	m_goal1 = ofRectangle(46, 274, 24, 225);
	m_goal2 = ofRectangle(1210, 274, 24, 225);


	//UI vars
	m_fontLg.load("NotoSans-Medium.ttf", 20);
	m_fontMd.load("NotoSans-Regular.ttf", 16);
	m_fontSm.load("NotoSans-Regular.ttf", 12);
	m_clockFont.load("RobotoMono-SemiBold.ttf", 36);
	m_titleImg.load("Start.png");
	m_instructionsImg.load("Instructions.png");
	m_setupImg.load("Setup.png");
	m_debugImg.load("debug.png");
	m_gameScreenImg.load("Arena.png");
	m_gameOverImg.load("GameOver.png");
	m_restartBoxImg.load("restartBox.png");
	m_crashImg.load("Crash.png");


	//UI button setup
	m_startButton.m_ui_setup();
	m_startButton.set(Config::APP_WINDOW_WIDTH / 2.0f - 100.0f, Config::APP_WINDOW_HEIGHT / 2.0f + 70.0f, 200.0f, 60.0f, 8.0f);
	
	m_instructionsButton.m_ui_setup();
	m_instructionsButton.set(1070.0f, 630.0f, 155.0f, 45.0f, 8.0f);
	
	m_instructionsBackButton.m_ui_setup();
	m_instructionsBackButton.set(50.0f, 630.0f, 155.0f, 45.0f, 8.0f);

	m_setupBackButton.m_ui_setup();
	m_setupBackButton.set(50.0f, 630.0f, 155.0f, 45.0f, 8.0f);

	m_setupButton.m_ui_setup();
	m_setupButton.set(1070.0f, 630.0f, 155.0f, 45.0f, 8.0f);

	m_restartButton.m_ui_setup();
	m_restartButton.set(Config::APP_WINDOW_WIDTH / 2.0f - 75.0f, Config::APP_WINDOW_HEIGHT / 2.0f + 100.0f, 150.0f, 60.0f, 8.0f);

	m_resumeGameButton.m_ui_setup();
	m_resumeGameButton.set(1070.0f, 630.0f, 155.0f, 45.0f, 8.0f);

	//ratio for converting between video resolution and game screen resolution
	m_widthRatio = Config::APP_WINDOW_WIDTH / Config::VIDEO_WIDTH;
	m_heightRatio = Config::APP_WINDOW_HEIGHT / Config::VIDEO_HEIGHT;

	//init GUI
	m_gui.setup();

	//ImGui font
	m_imFont = m_gui.addFont("NotoSans-Regular.ttf", 20, nullptr);
}

//--------------------------------------------------------------
void ofApp::update(){
	
	//OpenCV image processing ------------------------------------------------------------------------------------------------------------------
	
	//using a demo video
	if (!m_isLiveVideo)
	{
		m_videoPlayer.update(); //show new video frames, if any

		if (m_videoPlayer.isFrameNew()) //check for new frame
		{	
			m_origImage.setFromPixels(m_videoPlayer.getPixels()); //copy pixels from frame to cv image
			m_origImage.mirror(false, true); //mirror image
			m_resizeColImage.scaleIntoMe(m_origImage); //scale image into smaller size for setup/debug

			if (m_isBgSubtracting)
			{
				m_grayscaleImage = m_resizeColImage;
				if (m_captureBg == true)
				{
					m_grayscaleBg = m_grayscaleImage;
					m_captureBg = false;
				}
				processBgSubtract();
			}
			else if (m_isColorTracking)
			{
				processColor();
			}

		}
	}
	//using a webcam 
	if (m_isLiveVideo)
	{
		m_videoGrabber.update();

		if (m_videoGrabber.isFrameNew())//check for new frame
		{
			m_origImage.setFromPixels(m_videoGrabber.getPixels());
			m_origImage.mirror(false, true); //mirror image
			m_resizeColImage.scaleIntoMe(m_origImage); //scale image into smaller size for setup/debug

			if (m_isBgSubtracting)
			{
				m_grayscaleImage = m_resizeColImage;
				if (m_captureBg == true)
				{
					m_grayscaleBg = m_grayscaleImage;
					m_captureBg = false;
				}
				processBgSubtract();
			}
			else if (m_isColorTracking)
			{
				processColor();
			}
		}
	}



	//setting the player positions from the detected contours ------------------------------------------------------------------------------------------------------------------

	if (m_contourFinder.blobs.size() > 0)
	{
		ofVec2f pt;
		pt.set(m_contourFinder.blobs[0].boundingRect.getCenter().x, m_contourFinder.blobs[0].boundingRect.getCenter().y);
		pt.set(pt.x * m_widthRatio, pt.y * m_heightRatio);
		m_player1.m_pos.set(pt);
		m_player1.m_box = m_contourFinder.blobs[0].boundingRect;
	}
	if (m_contourFinder2.blobs.size() > 0)
	{
		ofVec2f pt;
		pt.set(m_contourFinder2.blobs[0].boundingRect.getCenter().x, m_contourFinder2.blobs[0].boundingRect.getCenter().y);
		pt.set(pt.x * m_widthRatio, pt.y * m_heightRatio);
		m_player2.m_pos.set(pt);
		m_player2.m_box = m_contourFinder2.blobs[0].boundingRect;
	}


	//GUI event handling ------------------------------------------------------------------------------------------------------------------

	if(m_startButton.isMousePressed())
	{
		m_isTitle = false;
		m_isInstructions = true;
	}
	if (m_instructionsBackButton.isMousePressed())
	{
		m_isInstructions = false;
		m_isTitle = true;
	}
	if (m_instructionsButton.isMousePressed())
	{
		m_isInstructions = false;
		m_isSetup = true;
	}
	if (m_setupBackButton.isMousePressed())
	{
		m_isSetup = false;
		m_isInstructions = true;
		m_isPickingColor1 = false;
		m_isPickingColor2 = false;
	}
	if (m_setupButton.isMousePressed())
	{
		m_isSetup = false;
		m_isPlaying = true;
		ofResetElapsedTimeCounter();
		m_isPickingColor1 = false;
		m_isPickingColor2 = false;
	}	
	if (m_restartButton.isMousePressed())
	{
		m_gameOver = false;
		m_isPlaying = true;
		m_gameOverSfxPlayed = false;
		restartGame();
	}
	if (m_resumeGameButton.isMousePressed())
	{
		m_isDebugging = false;
		m_isPlaying = true;
		restartGame();
	}

	//state update handling ------------------------------------------------------------------------------------------------------------------
	
	//setting button enabled states based on which screen we are on
	if (m_isTitle)
	{
		m_startButton.enabled = true;
		m_instructionsBackButton.enabled = false;
		m_instructionsButton.enabled = false;
		m_setupButton.enabled = false;
		m_setupBackButton.enabled = false;
		m_restartButton.enabled = false;
		m_resumeGameButton.enabled = false;
	}
	else if (m_isInstructions)
	{
		m_startButton.enabled = false;
		m_instructionsBackButton.enabled = true;
		m_instructionsButton.enabled = true;
		m_setupButton.enabled = false;
		m_setupBackButton.enabled = false;
		m_restartButton.enabled = false;
		m_resumeGameButton.enabled = false;
	}
	else if (m_isSetup)
	{
		m_startButton.enabled = false;
		m_instructionsBackButton.enabled = false;
		m_instructionsButton.enabled = false;
		m_setupButton.enabled = true;
		m_setupBackButton.enabled = true;
		m_restartButton.enabled = false;
		m_resumeGameButton.enabled = false;
	}
	else if (m_gameOver)
	{
		m_startButton.enabled = false;
		m_instructionsBackButton.enabled = false;
		m_instructionsButton.enabled = false;
		m_setupButton.enabled = false;
		m_setupBackButton.enabled = false;
		m_restartButton.enabled = true;
		m_resumeGameButton.enabled = false;
	}
	else if (m_isDebugging)
	{
		m_startButton.enabled = false;
		m_instructionsBackButton.enabled = false;
		m_instructionsButton.enabled = false;
		m_setupButton.enabled = false;
		m_setupBackButton.enabled = false;
		m_restartButton.enabled = false;
		m_resumeGameButton.enabled = true;
	}

	// Actual game updates ------------------------------------------------------------------------------------------------------------------

	else if (m_isPlaying)
	{
		//set button states
		m_startButton.enabled = false;
		m_instructionsBackButton.enabled = false;
		m_instructionsButton.enabled = false;
		m_setupButton.enabled = false;
		m_setupBackButton.enabled = false;
		m_restartButton.enabled = false;
		m_resumeGameButton.enabled = false;


		//check if ran out of time
		if (m_time <= 0)
		{
			m_gameOver = true; //if yes, game is over
			m_isPlaying = false;
		}
		//update the countdown time
		m_time = m_startTime - ofGetElapsedTimef();

		//move the ball and switch direction if hitting arena box edge
		m_ball.update(m_arena);
		//check for collision between ball and players
		m_ball.checkPlayerCollision(m_player1.m_pos, m_player1.m_radius);
		m_ball.checkPlayerCollision(m_player2.m_pos, m_player2.m_radius);
		m_ball.checkPlayerCollision(ofVec2f(ofGetMouseX(), ofGetMouseY()), 5.0f);
		//check for goal box collision
		if (((m_ball.m_pos.x) <= m_goal1.width + m_goal1.x + m_ball.m_radius) &&
			  ((m_ball.m_pos.y)	>= m_goal1.y + m_ball.m_radius) && 
				(m_ball.m_pos.y <= m_goal1.y + m_goal1.height + m_ball.m_radius)
			)
		{
			//play SFX, update score, and reset ball position
			m_goalPlayer.play();
			m_player2Score++;
			m_ball.m_pos = m_ball.m_origPos;
			m_ball.setZeroSpeed();
		}
		if (((m_ball.m_pos.x) >= m_goal2.x - m_ball.m_radius) &&
			((m_ball.m_pos.y) >= m_goal2.y + m_ball.m_radius) &&
			(m_ball.m_pos.y <= m_goal2.y + m_goal2.height + m_ball.m_radius)
			)
		{
			//play SFX, update score, and reset ball position
			m_goalPlayer.play();
			m_player1Score++;
			m_ball.m_pos = m_ball.m_origPos;
			m_ball.setZeroSpeed();
		}

		//play collision sound if the players are close
		if (!m_bonkPlayer.isPlaying() && m_playersClose)
			m_bonkPlayer.play();
	}

	//Player detection and collision events ------------------------------------------------------------------------------------------------------------------
	
	//checking for players being close to each other
	int dist = ofDist(m_player1.m_pos.x, m_player2.m_pos.x, m_player1.m_pos.y, m_player2.m_pos.y);
	if ((dist < (m_player1.m_radius + m_player2.m_radius + 20.0f)) && m_contourFinder.blobs.size() > 0 && m_contourFinder2.blobs.size() > 0)
	{
		m_playersClose = true;
		m_crashPos = ofVec2f((m_player1.m_pos.x + m_player2.m_pos.x) / 2.0f, (m_player1.m_pos.y + m_player2.m_pos.y) / 2.0f);
	}
	else if (m_player1.m_box.intersects(m_player2.m_box))
	{
		m_playersClose = true;
	}
	else
	{
		m_playersClose = false;
	}


	//checking for player detection for players 1 and 2
	if (m_contourFinder.blobs.size() < 1)
	{
		m_player1Lost = true;
		m_player1Detected = false;
	}
	if (m_contourFinder.blobs.size() == 1)
	{
		m_player1Detected = true;
		m_player1Lost = false;
	}
	if (m_contourFinder2.blobs.size() < 1)
	{
		m_player2Lost = true;
		m_player2Detected = false;
	}
	if (m_contourFinder2.blobs.size() == 1)
	{
		m_player2Detected = true;
		m_player2Lost = false;
	}

	//determing the crash position - middle point between each player position
	m_crashPos = ofVec2f((m_player1.m_pos.x + m_player2.m_pos.x) / 2.0f, (m_player1.m_pos.y + m_player2.m_pos.y) / 2.0f);
	
	//resizing the player paddle based on the contour area
	float player1rad = sqrt(m_player1.m_box.getArea() / 3.14159);
	m_player1.m_radius = player1rad;
	float player2rad = sqrt(m_player2.m_box.getArea() / 3.14159);
	m_player2.m_radius = player2rad;

	//play the game over sound if the sound is not yet played and the game is finished
	if (m_gameOver && !m_gameOverSfxPlayed)
	{
		m_gameOverPlayer.play();
		m_gameOverSfxPlayed = true;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	
	//ImGui setup

	m_gui.begin(); 

	ImGuiStyle& style = ImGui::GetStyle();
	style.GrabRounding = style.FrameRounding = 4.0f;
	ImGui::StyleColorsDark();
	style.ItemSpacing = ImVec2(8.0f, 8.0f);
	style.ScrollbarRounding = 8.0f;
	style.WindowRounding = 8.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);



	//show control settings for the OpenCV image processing -------------------------------------------------------------------------------------------------------------------------------

	if (m_isDebugging)
	{
		//UI background image
		m_debugImg.draw(0, 0);
		//Object tracking images
		m_grayscaleDiff.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN); //top-left image
		m_resizeColImage.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN); //top-right image
		m_grayscaleBg.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT); //bottom-left image
		m_grayscaleImage.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT); //bottom-right image
		if (m_isColorTracking)
		{
			m_diffImage.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
			m_maskedImage1.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
			m_maskedImage2.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
		}
		else if (m_isBgSubtracting)
		{
			m_maskedImage2.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
			m_maskedImage1.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
		}


		//draw UI box with text to indicate separate player contours

		ofSetColor(50);
		ofDrawRectangle(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 40.0f, 150.0f, 30.0f);
		ofDrawRectangle(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 40.0f, 150.0f, 30.0f);
		ofSetColor(255);

		m_fontSm.drawString("Player 1 contour", Config::SETUP_LEFT_MARGIN + 10.0f, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 20.0f);
		m_fontSm.drawString("Player 2 contour", Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH + 10.0f, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 20.0f);
		ofSetColor(255);


		//UI buttons

		m_resumeGameButton.m_ui_draw();
		m_fontMd.drawString("Resume", 1110.0f, 660.0f);


		//Drawing contours and center points ------------------------------------------------------------------------------------------------------------------

		//draw contours
		static ofVec2f contourCenter;
		static float contourArea = 0.0f;

		// for background subtraction
		if (!m_isColorTracking)
		{
			ofPushMatrix();
			{
				ofTranslate(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
				for (int i = 0; i < m_contourFinder.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder.blobs[i].boundingRect.getCenter().x,
						m_contourFinder.blobs[i].boundingRect.getCenter().y);

					contourArea = m_contourFinder.blobs[i].area;

					//draw contour
					m_contourFinder.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::red);
					ofDrawCircle(contourCenter.x, contourCenter.y, 5.0f);

					//draw text
					ofSetColor(ofColor::purple);
					ofDrawBitmapString("Player 1: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourArea),
						m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
				//contour 2
				for (int i = 0; i < m_contourFinder2.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder2.blobs[i].boundingRect.getCenter().x,
						m_contourFinder2.blobs[i].boundingRect.getCenter().y);

					contourArea = m_contourFinder2.blobs[i].area;

					//draw contour
					m_contourFinder2.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::green);
					ofDrawCircle(contourCenter.x, contourCenter.y, 5.0f);

					//draw text
					ofSetColor(ofColor::purple);
					ofDrawBitmapString("Player 2: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourArea),
						m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
			}
			ofPopMatrix();
		}

		// for color-tracking
		else
		{
			ofPushMatrix();
			{
				ofTranslate(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
				for (int i = 0; i < m_contourFinder.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder.blobs[i].boundingRect.getCenter().x, m_contourFinder.blobs[i].boundingRect.getCenter().y);
					contourArea = m_contourFinder.blobs[i].area;

					//draw contour
					m_contourFinder.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::fuchsia);
					ofDrawCircle(contourCenter.x, contourCenter.y, 10.0f);

					//draw textual information
					ofSetColor(ofColor::darkGreen);
					ofDrawBitmapString("Player 1: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourCenter.x), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
				for (int i = 0; i < m_contourFinder2.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder2.blobs[i].boundingRect.getCenter().x, m_contourFinder2.blobs[i].boundingRect.getCenter().y);
					contourArea = m_contourFinder2.blobs[i].area;

					//draw contour
					m_contourFinder2.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::green);
					ofDrawCircle(contourCenter.x, contourCenter.y, 10.0f);

					//draw textual information
					ofSetColor(ofColor::darkGreen);
					ofDrawBitmapString("Player 2: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourCenter.x), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
			}
			ofPopMatrix();
		}

		ImGui::SetNextWindowPos(ofVec2f(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE * 4.0f + Config::VIDEO_WIDTH * 2.0f, Config::SETUP_TOP_MARGIN), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ofVec2f(500.0f, 470.0f), ImGuiCond_Once);
		ImGui::Begin("Calibration Settings");
		{
			ImGui::Spacing();
			ImGui::Text("Calibrate the settings to optimize object detection.");
			ImGui::Text("Adjust threshold levels to clean the object contours.");
			ImGui::Dummy(ofVec2f(0.0f, 15.0f));
			if (ImGui::RadioButton("Toggle live video/camera and demo video", m_isLiveVideo))
			{
				m_isLiveVideo = !m_isLiveVideo;
			}
			ImGui::Separator();
			ImGui::SliderInt("Threshold", &m_threshold, 1, 255);
			ImGui::SliderFloat("Minimum Area", &m_minArea, 0.0f, Config::VIDEO_WIDTH * Config::VIDEO_HEIGHT);
			ImGui::SliderFloat("Maximum Area", &m_maxArea, 0.0f, Config::VIDEO_WIDTH * Config::VIDEO_HEIGHT);
			ImGui::Spacing();
			ImGui::SeparatorText("Object Tracking Mode");
			ImGui::Text("Choose between background subtraction or color-tracking for \nobject tracking.");
			ImGui::Spacing();
			if (ImGui::RadioButton("Background subtraction", m_isBgSubtracting))
			{
				m_isBgSubtracting = true;
				m_isColorTracking = false;
			}
			if (ImGui::RadioButton("Color tracking", m_isColorTracking))
			{
				m_isColorTracking = true;
				m_isBgSubtracting = false;
			}

			if (ImGui::Button("Capture new background image"))
			{
				m_captureBg = true;
			}

			ImGui::Text("Press SPACEBAR to pause the video if viewing demo video.");

			if (m_isColorTracking)
			{
				ImGui::SeparatorText("Color selection");
				ImGui::Dummy(ofVec2f(0.0f, 10.0f));
				ImGui::Text("Right-click over the color in the video you wish to select.");
				ImGui::ColorEdit3("Player 1 Color", &m_trackedColor_1[0]);
				if (ImGui::RadioButton("Picking color 1", m_isPickingColor1))
				{
					m_isPickingColor1 = true;
					m_isPickingColor2 = false;
				}
				ImGui::ColorEdit3("Player 2 Color", &m_trackedColor_2[0]);
				if (ImGui::RadioButton("Picking color 2", m_isPickingColor2))
				{
					m_isPickingColor2 = true;
					m_isPickingColor1 = false;
				}
			}
		}
		ImGui::End();
	}


	//Title screen UI -----------------------------------------------------------------------------------------------------------------------------------	
	
	else if (m_isTitle)
	{
		m_titleImg.draw(0, 0);
		m_startButton.m_ui_draw();
		m_fontLg.drawString("Start", m_startButton.x + m_startButton.width / 3.0f, m_startButton.y + m_startButton.height / 2.0f + 10.f);
	}

	// Instructions screen UI -----------------------------------------------------------------------------------------------------------------------------------	

	else if (m_isInstructions)
	{
		m_instructionsImg.draw(0, 0);
		ofSetColor(Config::IDLE_DARK_COL);
		ofSetColor(255);
		
		m_instructionsBackButton.m_ui_draw();
		m_fontMd.drawString("Back", 100.0f, 660.0f);

		m_instructionsButton.m_ui_draw();
		m_fontMd.drawString("Next", 1123.0f, 660.0f);
	}
	
	//Setup screen UI -------------------------------------------------------------------------------------------------------------------------------------------

	else if (m_isSetup)
	{
		//UI background image
		m_setupImg.draw(0, 0);

		//Object tracking images
		m_grayscaleDiff.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN); //top-left image
		m_resizeColImage.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN); //top-right image
		m_grayscaleBg.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT); //bottom-left image
		m_grayscaleImage.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT); //bottom-right image
		if (m_isColorTracking)
		{
			m_diffImage.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
			m_maskedImage1.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
			m_maskedImage2.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
		}
		else if (m_isBgSubtracting)
		{
			m_maskedImage2.draw(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
			m_maskedImage1.draw(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_HEIGHT);
		}


		//Draw UI box and text to indicate each player contour

		ofSetColor(50);
		ofDrawRectangle(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 40.0f, 150.0f, 30.0f);
		ofDrawRectangle(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 40.0f, 150.0f, 30.0f);
		ofSetColor(255);

		m_fontSm.drawString("Player 1 contour", Config::SETUP_LEFT_MARGIN + 10.0f, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 20.0f);
		m_fontSm.drawString("Player 2 contour", Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH + 10.0f, Config::SETUP_TOP_MARGIN + Config::VIDEO_BORDER_SIZE * 2.0f + Config::VIDEO_HEIGHT * 2.0f - 20.0f);
		ofSetColor(255);


		//UI buttons
		m_setupBackButton.m_ui_draw();
		m_fontMd.drawString("Back", 100.0f, 660.0f);

		m_setupButton.m_ui_draw();
		m_fontMd.drawString("Start", 1123.0f, 660.0f);


		//Drawing contours and center points -------------------------------------------------------------------------------------------------------------------------------------------

		//draw contours
		static ofVec2f contourCenter;
		static float contourArea = 0.0f;

		//for background subtraction
		if (!m_isColorTracking)
		{
			ofPushMatrix();
			{
				//shift everything over
				ofTranslate(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
				//loop through contours and draw them
				for (int i = 0; i < m_contourFinder.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder.blobs[i].boundingRect.getCenter().x,
						m_contourFinder.blobs[i].boundingRect.getCenter().y);

					contourArea = m_contourFinder.blobs[i].area;

					//draw contour
					m_contourFinder.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::red);
					ofDrawCircle(contourCenter.x, contourCenter.y, 5.0f);

					//draw text
					ofSetColor(ofColor::purple);
					ofDrawBitmapString("Player 1: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourArea),
						m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
				//contour 2
				for (int i = 0; i < m_contourFinder2.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder2.blobs[i].boundingRect.getCenter().x,
						m_contourFinder2.blobs[i].boundingRect.getCenter().y);

					contourArea = m_contourFinder2.blobs[i].area;

					//draw contour
					m_contourFinder2.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::green);
					ofDrawCircle(contourCenter.x, contourCenter.y, 5.0f);

					//draw text
					ofSetColor(ofColor::purple);
					ofDrawBitmapString("Player 2: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourArea),
						m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
			}
			ofPopMatrix();
		}

		//for color-tracking
		else
		{
			ofPushMatrix();
			{
				ofTranslate(Config::SETUP_LEFT_MARGIN, Config::SETUP_TOP_MARGIN);
				for (int i = 0; i < m_contourFinder.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder.blobs[i].boundingRect.getCenter().x, m_contourFinder.blobs[i].boundingRect.getCenter().y);
					contourArea = m_contourFinder.blobs[i].area;

					//draw contour
					m_contourFinder.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::red);
					ofDrawCircle(contourCenter.x, contourCenter.y, 10.0f);

					//draw textual information
					ofSetColor(ofColor::darkGreen);
					ofDrawBitmapString("Player 1: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourCenter.x), m_contourFinder.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
				for (int i = 0; i < m_contourFinder2.blobs.size(); i++)
				{
					contourCenter.set(m_contourFinder2.blobs[i].boundingRect.getCenter().x, m_contourFinder2.blobs[i].boundingRect.getCenter().y);
					contourArea = m_contourFinder2.blobs[i].area;

					//draw contour
					m_contourFinder2.blobs[i].draw();

					//draw center
					ofSetColor(ofColor::green);
					ofDrawCircle(contourCenter.x, contourCenter.y, 10.0f);

					//draw textual information
					ofSetColor(ofColor::darkGreen);
					ofDrawBitmapString("Player 2: " + ofToString(contourCenter.x) + ", " + ofToString(contourCenter.y), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 20.0f);
					ofDrawBitmapString("Area: " + ofToString(contourCenter.x), m_contourFinder2.blobs[i].boundingRect.getMaxX() + Config::VIDEO_BORDER_SIZE, contourCenter.y + 40.0f);
				}
			}
			ofPopMatrix();
		}
		
		ImGui::SetNextWindowPos(ofVec2f(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE * 4.0f + Config::VIDEO_WIDTH * 2.0f, Config::SETUP_TOP_MARGIN), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ofVec2f(500.0f, 470.0f), ImGuiCond_Once);
		ImGui::Begin("Calibration Settings");
		{
			ImGui::Spacing();
			ImGui::Text("Calibrate the settings to optimize object detection.");
			ImGui::Text("Adjust threshold levels to clean the object contours.");
			ImGui::Dummy(ofVec2f(0.0f, 15.0f));
			if (ImGui::RadioButton("Toggle live video/camera and demo video", m_isLiveVideo))
			{
				m_isLiveVideo = !m_isLiveVideo;
			}
			ImGui::Separator();
			ImGui::SliderInt("Threshold", &m_threshold, 1, 255);
			ImGui::SliderFloat("Minimum Area", &m_minArea, 0.0f, Config::VIDEO_WIDTH* Config::VIDEO_HEIGHT);
			ImGui::SliderFloat("Maximum Area", &m_maxArea, 0.0f, Config::VIDEO_WIDTH* Config::VIDEO_HEIGHT);
			ImGui::Spacing();
			ImGui::SeparatorText("Object Tracking Mode");
			ImGui::Text("Choose between background subtraction or color-tracking for \nobject tracking.");
			ImGui::Spacing();
			if (ImGui::RadioButton("Background subtraction", m_isBgSubtracting))
			{
				m_isBgSubtracting = true;
				m_isColorTracking = false;
			}
			if (ImGui::RadioButton("Color tracking", m_isColorTracking))
			{
				m_isColorTracking = true;
				m_isBgSubtracting = false;
			}

			if (ImGui::Button("Capture new background image"))
			{
				m_captureBg = true;
			}
			
			ImGui::Text("Press SPACEBAR to pause the video if viewing demo video.");
			
			if (m_isColorTracking)
			{
				ImGui::SeparatorText("Color selection");
				ImGui::Dummy(ofVec2f(0.0f, 10.0f));
				ImGui::Text("Right-click over the color in the video you wish to select.");
				ImGui::ColorEdit3("Player 1 Color", &m_trackedColor_1[0]);
				if (ImGui::RadioButton("Picking color 1", m_isPickingColor1))
				{
					m_isPickingColor1 = true;
					m_isPickingColor2 = false;
				}
				ImGui::ColorEdit3("Player 2 Color", &m_trackedColor_2[0]);
				if (ImGui::RadioButton("Picking color 2", m_isPickingColor2))
				{
					m_isPickingColor2 = true;
					m_isPickingColor1 = false;
				}
			}
		}
		ImGui::End();

		
	}

	//Active game UI - hockey rink -------------------------------------------------------------------------------------------------------------------------------------------

	else if (m_isPlaying)
	{
		ofSetColor(255);
		m_gameScreenImg.draw(0, 0);

		ofSetColor(238, 250, 255);
		ofDrawRectangle(m_goal1);
		ofDrawRectangle(m_goal2);
		
		//draw the countdown timer and scores
		ofSetColor(255);
		if (m_time >= 10)
			m_clockFont.drawString(ofToString(m_time), ofGetWidth() / 2.0f - 29.0f, m_arena.y - 10.0f);
		else if (m_time < 10)
			m_clockFont.drawString(ofToString(m_time), ofGetWidth() / 2.0f - 14.0f, m_arena.y - 10.0f);
		m_clockFont.drawString(ofToString(m_player1Score), m_arena.x + 57.0f, m_arena.y - 10.0f);
		m_clockFont.drawString(ofToString(m_player2Score), m_arena.x + m_arena.width - 80.0f, m_arena.y - 10.0f);

		//drawing the ball and players
		m_ball.draw();
		m_player1.draw();
		m_player2.draw();

		//drawing the detection events - lost, detected
		ofSetColor(0);
		if(m_player1Detected)
			m_fontMd.drawString("Player 1 is active", m_arena.x + 130.0f, m_arena.y - 10.0f);
		else if(m_player1Lost)
			m_fontMd.drawString("Player 1 is lost", m_arena.x + 130.0f, m_arena.y - 10.0f);
		if (m_player2Detected)
			m_fontMd.drawString("Player 2 is active", m_arena.x + m_arena.width - 300.0f, m_arena.y - 10.0f);
		else if (m_player2Lost)
			m_fontMd.drawString("Player 2 is lost", m_arena.x + m_arena.width - 280.0f, m_arena.y - 10.0f);

		//UI text for being too close or too far
		if (m_player1.m_radius < 10.0f || m_player2.m_radius < 10.0f)
			m_fontSm.drawString("Too far from the camera", m_arena.x + m_arena.width - 270.0f, Config::APP_WINDOW_HEIGHT - 12.0f);
		else if(m_player1.m_radius > 70.0f || m_player2.m_radius > 70.0f)
			m_fontSm.drawString("Too close to the camera", m_arena.x + m_arena.width - 270.0f, Config::APP_WINDOW_HEIGHT - 12.0f);

		ofSetColor(255);
		//drawing the close proximity event - a crash image
		if (m_playersClose)
		{
			ofSetRectMode(OF_RECTMODE_CENTER);
			m_crashImg.draw(m_crashPos);
			ofSetRectMode(OF_RECTMODE_CORNER);
		}

		ofSetColor(0);
		m_fontSm.drawString("Press TAB to view debug settings.", 70.0f, Config::APP_WINDOW_HEIGHT - 12.0f);
		ofSetColor(255);
	}

	//Game over screen UI -------------------------------------------------------------------------------------------------------------------------------------------

	else if (m_gameOver)
	{

		ofSetColor(255);
		m_gameOverImg.draw(0, 0); //background image

		//draw faded out time and scores
		ofSetColor(170);
		if(m_time >= 10)
			m_clockFont.drawString(ofToString(m_time), ofGetWidth() / 2.0f - 29.0f, m_arena.y - 10.0f);
		else if (m_time < 10)
			m_clockFont.drawString(ofToString(m_time), ofGetWidth() / 2.0f - 14.0f, m_arena.y - 10.0f);
		m_clockFont.drawString(ofToString(m_player1Score), m_arena.x + 57.0f, m_arena.y - 10.0f);
		m_clockFont.drawString(ofToString(m_player2Score), m_arena.x + m_arena.width - 80.0f, m_arena.y - 10.0f);

		m_restartButton.m_ui_draw();
		m_fontMd.drawString("Play again?", Config::APP_WINDOW_WIDTH / 2.0f - 55.0f, Config::APP_WINDOW_HEIGHT / 2.0f + 138.0f);

		ofSetColor(0);
		if(m_player1Score > m_player2Score)
			m_fontLg.drawString("Player 1 wins!", Config::APP_WINDOW_WIDTH / 2.0f - 78.0f, Config::APP_WINDOW_HEIGHT / 2.0f - 100.0f);
		else if (m_player1Score < m_player2Score)
			m_fontLg.drawString("Player 2 wins!", Config::APP_WINDOW_WIDTH / 2.0f - 78.0f, Config::APP_WINDOW_HEIGHT / 2.0f - 100.0f);
		else if(m_player1Score == m_player2Score)
			m_fontLg.drawString("It's a tie", Config::APP_WINDOW_WIDTH / 2.0f - 55.0f, Config::APP_WINDOW_HEIGHT / 2.0f - 100.0f);
		ofSetColor(255);
	}

	//color picking UI
	if (m_isPickingColor1 || m_isPickingColor2)
	{
		ofPushMatrix();
		{
			ofTranslate(ofGetMouseX(), ofGetMouseY());
			m_screenshotImg.grabScreen(0, 0, Config::APP_WINDOW_WIDTH, Config::APP_WINDOW_HEIGHT);
			m_hoverColor = m_screenshotImg.getColor(ofGetMouseX(), ofGetMouseY());
			ofSetColor(m_hoverColor);
			ofDrawRectangle(5,5, 50.0f, 50.0f);
		}
		ofPopMatrix();
	}

	m_gui.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	//add some video controls, e.g. pause toggle
	if (key == 32 && (m_isDebugging || m_isSetup) ) // or 32 (spacebar)
	{
		m_videoPlayer.setPaused(!m_videoPlayer.isPaused());
	}
	if (key == OF_KEY_TAB && m_isPlaying)
	{
		m_isDebugging = true;
		m_isPlaying = false;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (button == OF_MOUSE_BUTTON_RIGHT && m_isColorTracking)
	{
		if (m_isPickingColor1)
		{
			ofRectangle videoRect;
			if (m_isSetup || m_isDebugging)
			{
				videoRect = ofRectangle(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH,
										Config::SETUP_TOP_MARGIN,
										Config::VIDEO_WIDTH,
										Config::VIDEO_HEIGHT);
			}
			//step 1 - clamp
			int convertScreenToVideoSpace_X = ofClamp(x, videoRect.getMinX(), videoRect.getMaxX());
			int convertScreenToVideoSpace_Y = ofClamp(y, videoRect.getMinY(), videoRect.getMaxY());

			//step 2 - shift to image space
			convertScreenToVideoSpace_X -= videoRect.getMinX();
			convertScreenToVideoSpace_Y -= videoRect.getMinY();

			//step 3 - find that pixel
			const int index = (convertScreenToVideoSpace_X + convertScreenToVideoSpace_Y * Config::VIDEO_WIDTH) * m_resizeColImage.getPixels().getBytesPerPixel(); //m_colorImage.getPixels().getBytesPerPixel() is 3 
			m_trackedColor_1[0] = m_resizeColImage.getPixels()[index + 0] / 255.0f; // R, and we convert to a "float" color [0.0f, 1.0f]
			m_trackedColor_1[1] = m_resizeColImage.getPixels()[index + 1] / 255.0f; // G
			m_trackedColor_1[2] = m_resizeColImage.getPixels()[index + 2] / 255.0f; // B

			m_isPickingColor1 = false;

		}
		else if (m_isPickingColor2)
		{
			ofRectangle videoRect;
			if (m_isSetup || m_isDebugging)
			{
				videoRect = ofRectangle(Config::SETUP_LEFT_MARGIN + Config::VIDEO_BORDER_SIZE + Config::VIDEO_WIDTH,
					Config::SETUP_TOP_MARGIN,
					Config::VIDEO_WIDTH,
					Config::VIDEO_HEIGHT);
			}
			//step 1 - clamp
			int convertScreenToVideoSpace_X = ofClamp(x, videoRect.getMinX(), videoRect.getMaxX());
			int convertScreenToVideoSpace_Y = ofClamp(y, videoRect.getMinY(), videoRect.getMaxY());

			//step 2 - shift to image space
			convertScreenToVideoSpace_X -= videoRect.getMinX();
			convertScreenToVideoSpace_Y -= videoRect.getMinY();

			//step 3 - find that pixel
			//use function: pixelIndex = x + y * width
			const int index = (convertScreenToVideoSpace_X + convertScreenToVideoSpace_Y * Config::VIDEO_WIDTH) * m_resizeColImage.getPixels().getBytesPerPixel(); //m_colorImage.getPixels().getBytesPerPixel() is 3 
			m_trackedColor_2[0] = m_resizeColImage.getPixels()[index + 0] / 255.0f; // R, and we convert to a "float" color [0.0f, 1.0f]
			m_trackedColor_2[1] = m_resizeColImage.getPixels()[index + 1] / 255.0f; // G
			m_trackedColor_2[2] = m_resizeColImage.getPixels()[index + 2] / 255.0f; // B

			m_isPickingColor2 = false;
		}
	}
}

void ofApp::processColor()
{
//loop through all pixels of color and determine if each pixel is that color (within threshold)
//if color found, write a white pixel, else write a black pixel to our grayscale image

	const int numChannels = Config::VIDEO_WIDTH * Config::VIDEO_HEIGHT * 3; 
	static float pixel[3] = { 0.0f, 0.0f, 0.0f };

	//////////////////////////////////////////		combined			////////////////////////////////////////

		//loop through all channels 
	for (int i = 0; i < numChannels; i += 3)
	{
		//define our current "pixel/color"
		pixel[0] = m_resizeColImage.getPixels()[i + 0]; //R
		pixel[1] = m_resizeColImage.getPixels()[i + 1]; //G
		pixel[2] = m_resizeColImage.getPixels()[i + 2]; //B

		//determine if the current pixel matches tracked color
		if (
			(
				abs(pixel[0] - m_trackedColor_1[0] * 255.0f) < m_threshold &&	//red
				abs(pixel[1] - m_trackedColor_1[1] * 255.0f) < m_threshold &&		//green
				abs(pixel[2] - m_trackedColor_1[2] * 255.0f) < m_threshold		//blue
			)
			||
			(
				abs(pixel[0] - m_trackedColor_2[0] * 255.0f) < m_threshold &&	//red
				abs(pixel[1] - m_trackedColor_2[1] * 255.0f) < m_threshold &&		//green
				abs(pixel[2] - m_trackedColor_2[2] * 255.0f) < m_threshold		//blue
			)
			)
		{
			//same color
			m_diffImage.getPixels()[i / 3] = 255; //write white pixel for positive
		}
		else
		{
			//not the same color
			m_diffImage.getPixels()[i / 3] = 0; //write black pixel for negative
		}
	}
	//update textures
	m_diffImage.flagImageChanged();


	//////////////////////////////////////////		contour 1			////////////////////////////////////////

	//loop through all channels 
	for (int i = 0; i < numChannels; i += 3)
	{
		//define our current "pixel/color"
		pixel[0] = m_resizeColImage.getPixels()[i + 0]; //R
		pixel[1] = m_resizeColImage.getPixels()[i + 1]; //G
		pixel[2] = m_resizeColImage.getPixels()[i + 2]; //B

		//determine if the current pixel matches tracked color
		if (
				(
					abs(pixel[0] - m_trackedColor_1[0] * 255.0f) < m_threshold &&	//red
					abs(pixel[1] - m_trackedColor_1[1] * 255.0f) < m_threshold &&		//green
					abs(pixel[2] - m_trackedColor_1[2] * 255.0f) < m_threshold		//blue
				)
			)
		{
			//same color
			m_maskedImage1.getPixels()[i / 3] = 255; //write white pixel for positive
		}
		else
		{
			//not the same color
			m_maskedImage1.getPixels()[i / 3] = 0; //write black pixel for negative
		}
	}
	//update textures
	m_maskedImage1.flagImageChanged();

	//find contours
	m_contourFinder.findContours(m_maskedImage1, m_minArea, m_maxArea, m_numContoursConsidered, false, true);

	//////////////////////////////////////////		contour 2			////////////////////////////////////////

	////loop through all channels 
	for (int i = 0; i < numChannels; i += 3)
	{
		//define our current "pixel/color"
		pixel[0] = m_resizeColImage.getPixels()[i + 0]; //R
		pixel[1] = m_resizeColImage.getPixels()[i + 1]; //G
		pixel[2] = m_resizeColImage.getPixels()[i + 2]; //B

		//determine if the current pixel matches our tracked color
		if (
			(
				abs(pixel[0] - m_trackedColor_2[0] * 255.0f) < m_threshold &&	//red
				abs(pixel[1] - m_trackedColor_2[1] * 255.0f) < m_threshold &&		//green
				abs(pixel[2] - m_trackedColor_2[2] * 255.0f) < m_threshold		//blue
				)
			)
		{
			//same color
			m_maskedImage2.getPixels()[i / 3] = 255; //write white pixel for positive
		}
		else
		{

			m_maskedImage2.getPixels()[i / 3] = 0; //write black pixel for negative

		}
	}
	//update textures
	m_maskedImage2.flagImageChanged();

	//find contours
	m_contourFinder2.findContours(m_maskedImage2, m_minArea, m_maxArea, m_numContoursConsidered, false, true);
	
}

//function performs background subtraction on image to find contours of objects
void ofApp::processBgSubtract()
{
	//get the difference between images
	m_grayscaleDiff.absDiff(m_grayscaleBg, m_grayscaleImage);
	//determine how much "gray" is considered
	m_grayscaleDiff.threshold(m_threshold);
	//look for contours/objects of interest
	m_contourFinder.findContours(m_grayscaleDiff, m_minArea, m_maxArea, m_numContoursConsidered, false, true);

	//create grayscale diff images to isolate each player contour
	m_maskedImage1 = m_grayscaleDiff;
	m_maskedImage2 = m_grayscaleDiff;

	//mask over the first found contour into the diff image for player 2
	for (int i = 0; i < m_contourFinder.blobs.size(); i++)
	{
		m_maskedImage1.drawBlobIntoMe(m_contourFinder.blobs[i], 0);
	}
	
	//find player 2's contour from the masked diff image
	m_contourFinder2.findContours(m_maskedImage1, m_minArea, m_maxArea, m_numContoursConsidered, false, true);

	//apply the second mask onto the first one
	for (int i = 0; i < m_contourFinder2.blobs.size(); i++)
	{
		m_maskedImage2.drawBlobIntoMe(m_contourFinder2.blobs[i], 0);
	}

	//find the first contour again from the 
	m_contourFinder.findContours(m_maskedImage2, m_minArea, m_maxArea, m_numContoursConsidered, false, true);

}

void ofApp::restartGame()
{
	ofResetElapsedTimeCounter();
	m_ball.reset();
	m_player1Score = 0;
	m_player2Score = 0;
	m_time = m_startTime;
}

