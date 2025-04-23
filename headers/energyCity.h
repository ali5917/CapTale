#ifndef ENERGYCITY_H
#define ENERGYCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include <cstdlib>

class Fruit {
    private:
        Vector2 position;
        Texture2D fruitTexture;
        float speed;
        bool active = false;
    
    public:
        Fruit () {};

        void initialize (const char* path, float s) {
            speed = s;
            fruitTexture = LoadTexture(path);
        }

        void update () {
            if (!active) return;

            position.y += speed;   
            if (position.y >= GetScreenHeight()) {
                active = false;
            } 
        }

        void draw () {
            if (active) {
                DrawTexture(fruitTexture, position.x, position.y, WHITE);
            }
        }

        void spawn () {
            position.x = rand() % (GetScreenWidth() - fruitTexture.width);
            position.y = 0;
            active = true;
        }

        bool isActive() {
            return active;
        }

        bool checkCollision (Rectangle basketRect) {
            Rectangle fruitRect = { position.x, position.y, (float)fruitTexture.width, (float)fruitTexture.height };
            bool horizontallyAligned = fruitRect.x + fruitRect.width > basketRect.x &&
                               fruitRect.x < basketRect.x + basketRect.width;
            bool verticallyTouching = fruitRect.y + fruitRect.height >= basketRect.y &&
                                    fruitRect.y + fruitRect.height <= basketRect.y + 1;

            return horizontallyAligned && verticallyTouching;
        }
};

class Basket {
    private:
        Vector2 position;
        Texture2D basketTexture;
        float speed;
    
    public:
        Basket (const char *path, Vector2 pos, float s) : position (pos), speed (s) {
            basketTexture = LoadTexture(path);
        };

        ~Basket () {
            UnloadTexture (basketTexture);
        }

        void update () {
            if (IsKeyDown(KEY_LEFT) && position.x > 0) {
                position.x -= speed;
            }

            if (IsKeyDown(KEY_RIGHT) && position.x < GetScreenWidth()) {
                position.x += speed;
            }
        }

        void draw () {
            DrawTexture(basketTexture, position.x, position.y, WHITE);
        }

        Rectangle getRect () {
            return {position.x, position.y, (float)basketTexture.width, (float)basketTexture.height };
        }
};

class EnergyCity {
    private:
        Texture2D background;
        int numFruits = 5;
        Fruit *fruits;
        Basket *basket;
        float spawnTimer = 0.0f;
        int currentFruit = 0;

    public:
        EnergyCity () {
            background = LoadTexture("assets/energyCity/bg1.png");

            fruits = new Fruit [numFruits];
            for (int i = 0; i < numFruits; i++) {
                fruits[i].initialize("assets/energyCity/apple.png", 5.0f);
            }

            basket = new Basket("assets/energyCity/basket.png", {600.0f, GetScreenHeight() - 154.0f - 10.0f}, 10.0f);
        }    

        ~EnergyCity () {
            UnloadTexture(background);
            delete [] fruits;
            delete basket;
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);
            for (int i = 0; i < numFruits; i++) {
                fruits[i].draw();
            }
            basket->draw();
        }

        void update () {
            spawnTimer += GetFrameTime();
            
            if (spawnTimer >= 1.0f) {
                spawnTimer = 0.0f;
            
                for (int i = 0; i < numFruits; i++) {
                    if (!fruits[i].isActive()) {
                        fruits[i].spawn();
                        break;
                    }
                }
            }
            
            for (int i = 0; i < numFruits; i++) {
                if (fruits[i].checkCollision(basket->getRect())) {
                    fruits[i].spawn();
                }
                fruits[i].update();
            }

            basket->update();
        }
};

#endif