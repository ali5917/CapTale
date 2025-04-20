#ifndef CUSTOMCITY_H
#define CUSTOMCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "cap.h"

class CustomCity {
    private:
        Texture capsList[4];
        int capsCount;
        int selectedIndex;
        Texture2D background;

    public: 
        CustomCity () {
            selectedIndex = 0;
            capsCount = 4;
            background = LoadTexture("assets/customCity/bg1.png");

            capsList[0] = LoadTexture("assets/customCity/cap0.png");
            capsList[1] = LoadTexture("assets/customCity/cap1.png");
            capsList[2] = LoadTexture("assets/customCity/cap2.png");
            capsList[3] = LoadTexture("assets/customCity/cap3.png");
        }

        ~CustomCity () {
            UnloadTexture(background);
            
            for (int i = 0; i < capsCount; i++) {
                UnloadTexture(capsList[i]);
            }
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);
            DrawTexture(capsList[selectedIndex],WINDOW_WIDTH/2 - capsList[selectedIndex].width/2, WINDOW_HEIGHT/2 - capsList[selectedIndex].height/2, WHITE);
        }

        int getSelectedCap () {
            return selectedIndex;
        }

        void nextCap () {
            selectedIndex += 1;
            if (selectedIndex == 4) selectedIndex = 0;
        }

        void prevCap () {
            selectedIndex -= 1;
            if (selectedIndex == -1) selectedIndex = 3;
        }
};

#endif
