#ifndef CUSTOMCITY_H
#define CUSTOMCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "cap.h"

class CustomCity {
    private:
        Cap capsList[4];
        int capsCount;
        int selectedIndex;
        Texture2D background;

    public: 
        CustomCity () {
            selectedIndex = 0;
            capsCount = 4;
            background = LoadTexture("assets/customCity/bg1.png");

            capsList[0] = Cap ("assets/customCity/cap0.png");
            capsList[1] = Cap ("assets/customCity/cap1.png");
            capsList[2] = Cap ("assets/customCity/cap2.png");
            capsList[3] = Cap ("assets/customCity/cap3.png");
        }

        ~CustomCity () {
            UnloadTexture(background);
            
            for (int i = 0; i < capsCount; i++) {
                capsList[i].unload();
            }
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);
            capsList[selectedIndex].draw(screenWidth / 2, screenHeight / 2);
        }

        Cap getSelectedCap () {
            return capsList[selectedIndex];
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