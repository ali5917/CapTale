#ifndef ENERGYCITY_H
#define ENERGYCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"
#include <cstdlib>

class Item {
    private:
        Vector2 position;
        Texture2D itemTexture;
        float speed;
        bool active = false;
    
    public:
        Item () {};

        void initialize (const char* path, float s) {
            speed = s;
            itemTexture = LoadTexture(path);
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
                DrawTexture(itemTexture, position.x, position.y, WHITE);
            }
        }

        void spawn () {
            position.x = rand() % (GetScreenWidth() - itemTexture.width);
            position.y = 0;
            active = true;
        }

        bool isActive() {
            return active;
        }

        bool checkCollision (Rectangle basketRect) {
            Rectangle itemRect = { position.x, position.y, (float)itemTexture.width, (float)itemTexture.height };
            bool horizontallyAligned = itemRect.x + itemRect.width > basketRect.x &&
                               itemRect.x < basketRect.x + basketRect.width;
            bool verticallyTouching = itemRect.y + itemRect.height >= basketRect.y &&
                                    itemRect.y + itemRect.height <= basketRect.y + 1;

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

            if (IsKeyDown(KEY_RIGHT) && position.x < GetScreenWidth() - basketTexture.width) {
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
        Cap *player;
        Texture2D background;
        int numItems = 6;
        Item *items;
        Basket *basket;
        float spawnTimer = 0.0f;
        int currentItem = 0;
        float energyCount;

        Font font = LoadFontEx("assets/fonts/Montserrat-SemiBold.ttf", TOKEN_FONT_SIZE, NULL, 0);

    public:
        EnergyCity (Cap *p) : player(p) {
            background = LoadTexture("assets/energyCity/bg1.png");

            items = new Item [numItems];
            for (int i = 0; i < numItems; i++) {
                string path = "assets/energyCity/item" + to_string(i+1) +  ".png";
                items[i].initialize(path.c_str(), 5.0f);
            }

            basket = new Basket("assets/energyCity/basket.png", {600.0f, GetScreenHeight() - 154.0f - 10.0f}, 10.0f);
        }    

        ~EnergyCity () {
            UnloadTexture(background);
            delete [] items;
            delete basket;
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);

            float barWidth = 300;
            float barHeight = 25;
            float energyPercent = player->getEnergy() / 100.0f;
            float x = WINDOW_WIDTH / 2 - barWidth / 2;
            float y = 90;
            string label = "Consumable Energy";
            Vector2 labelSize = MeasureTextEx(font, label.c_str(), TOKEN_FONT_SIZE/1.25, 0);
            DrawTextEx(font, label.c_str(), {WINDOW_WIDTH / 2 - labelSize.x / 2, y - TOKEN_FONT_SIZE / 2 - 20}, TOKEN_FONT_SIZE/1.25, 0, Color{0x00, 0x47, 0x65, 255});
            DrawRectangleRounded({x, y, barWidth, barHeight}, 0.5f, 10, Color{200, 220, 230, 255});
            DrawRectangleRounded({x, y, barWidth * energyPercent, barHeight}, 0.5f, 10, Color{0x00, 0x47, 0x65, 255});

            for (int i = 0; i < numItems; i++) {
                items[i].draw();
            }
            basket->draw();

        }

        void update () {
            spawnTimer += GetFrameTime();
            
            if (spawnTimer >= 1.0f) {
                spawnTimer = 0.0f;
            
                for (int i = 0; i < numItems; i++) {
                    if (!items[i].isActive()) {
                        items[i].spawn();
                        break;
                    }
                }
            }
            
            for (int i = 0; i < numItems; i++) {
                if (items[i].checkCollision(basket->getRect())) {
                    items[i].spawn();
                    player->increaseEnergy(2);
                }
                items[i].update();
            }
            basket->update();
        }
};

#endif