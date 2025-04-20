#ifndef CAP_H
#define CAP_H

#include "C:\raylib\raylib\src\raylib.h"

int screenWidth = 1920;
int screenHeight = 1080;

class Cap {
    private:
        Texture2D image;
        float x;
        float y;
    
    public:
        Cap () {}

        Cap (const char* path) {
            image = LoadTexture(path);
        }

        void draw(float x, float y) {
            this->x = x;
            this->y = y;

            DrawTexture(image, x - image.width / 2, y - image.height / 2, WHITE);
        }

        Texture2D getPosition () {
            return image;
        }

        void unload () {
            UnloadTexture(image);
        }
};

#endif