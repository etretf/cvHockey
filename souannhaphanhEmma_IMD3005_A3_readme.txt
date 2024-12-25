Emma Souannhaphanh
IMD3005A Sensor-based Interaction
A3: Visual Interaction 
Nov 9, 2023

Sources:

Addons:
ofxMSAInteractiveObject by memo - https://github.com/memo/ofxMSAInteractiveObject
ofxImGui
ofxOpenCv


Code:
Class code for OpenCV
https://openframeworks.cc/ofBook/chapters/OOPs!.html 
https://processing.org/examples/bouncybubbles.html 

SFX and music

https://pixabay.com/music/upbeat-space-chillout-14194/ 	- Space Chillout by penguinmusic
https://pixabay.com/sound-effects/success-1-6297/	- success 1 by Pixabay
https://pixabay.com/sound-effects/bonk-sound-effect-36055/	- Bonk by Pixabay 
https://pixabay.com/sound-effects/tada-fanfare-a-6313/		- Tada Fanfare A by Pixabay

Art: created by me in Figma and Illustrator


NOTE:
I created all the custom classes in the src folder - Ball, UI_Element, Player, Config. 

Overview:

I created a game that is a take on air hockey, except it's Computer Vision Hockey. The player(s) will be able to select either background subtraction or color tracking to track objects and their paddles. 
There will be two players since a game of air hockey is traditionally two-player. The player has the option to change the threshold, minimum and maximum area, and select colors for each player in the setup menu.
They can also toggle between using a demo video to see how the contours are tracked or using live video to play.

The player(s) will try to move their tracked object to move the paddle on screen. They need to hit the ball in the rink and try to 
get a higher score than their opponent. If either player is being actively detected, text on the screen will indicate it. If either player is not detected, text on the screen will indicate this.

If the players get too close (their tracked objects intersect or are within a calculated distance), a graphic depicting a crash effect will appear at the midpoint between the players.
There will be text to indicate if the players are either too close or too far from the camera.

The game will prompt the player to restart the game when the timer of 1 minute runs out. There are also debug settings accessible in the game by pressing TAB.


I chose this design because I wished to explore physics-based interactions with balls using OpenCV object tracking. Air hockey came to mind because the point scoring is not dependent on the player themself, but the defense of a net/box.
I implemented both background subtraction and color tracking methods to explore which method is more effective for my game and to give the player options. 


What was challenging:


Overall, this project was challenging. Computer vision object tracking is not very clean and it produces jittery information. I know we went over digital signal processsing filters in class, and I looked at the provided code, but I could not figure out how to make the tracking smoother with filters, so I left it. I also had difficulties with coding physics-based motion and animation myself since I have only really done a basic bouncing ball inside a rectangle before. 

Creating the UI was a bit easier since I reused some code I created for the previous assignment for creating my own buttons, but the UI work took a long while and it was very repetitive and easy to lose my place. 
There were issues with ImGui itself, but it did not break my game exactly. It just showed up once in a while and it did not seem related to my own code from looking at the call stack.

Another thing I found challenging was figuring out how to track my objects the way I wanted to. For example, object intersection would mean contours merging into one instead of having multiple contours. 

While trying to implement depth tracking, I realized that I wanted to affect my player objects by scaling them according to the area/radius. However, this meant that resizing the image over and over again caused the resolution to become ruined.

In all, this assignment was challenging.


What went well:

To deal with poor object tracking, I just tried making my source video cleaner by using my wall as the background and setting the threshold lower. I was also careful with keeping the lighting even while moving my objects since the color varies as I moved my objects around with my arms.

To figure out the ball collisions with multiple balls, I looked at Processing's bouncy bubbles example and the OOPs chapter in the ofBook. I was struggling with this for a while but then I eventually figured out my problems.

Using Figma to generate my background images was helpful so I could avoid drawing more things in the loops. Using some constants as guidelines also helped to place my UI, but UI still takes a long time to implement since I need to 
go pixel by pixel and sometimes run my app over and over to place things where I want them.

I did contour masking on additional ofxCvImages to avoid clumping contours on the same object. For example, I would find the contours first for the first player, then mask over a duplicate image and then run my contour finder on the masked image for the second player. Doing this, I was able to generate separate diff images to display to the player on the UI for them to see how the object detection and contour adjustments worked for each object.

To deal with the dynamic player paddle scaling, I resorted to drawing shapes in openFrameworks instead of using a loaded image. The downside was that I lost the drop shadow effect. Although I know this could be implemented using a shader or FBO most likely, I did not have time to explore this.

Overall, I am satisfied with my concept and my implementation of the UI and gameplay.





