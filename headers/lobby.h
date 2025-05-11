#ifndef LOBBY_H
#define LOBBY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"
#include "headers\message.h"
#include <iostream>
using namespace std;

enum RoomState {
    EMPTY,
    CUSTOMIZE,
    PONG,
    WITHDRAW,
    CAR,
    SPACESHOOTER,
    EARN,
    EAT
};

class Room {
    private:
        Vector2 pos;
        Vector2 size;
        Rectangle rec;
        RoomState gameState;

    public:
        Room(RoomState g, Vector2 p, Vector2 s) : pos(p), size(s), rec({pos.x, pos.y, size.x, size.y}), gameState(g) {}

        void drawRoom() {
            DrawRectangleRec(rec, BLANK);
        }

        Vector2 getCenter() {
            return Vector2({pos.x + size.x/2, pos.y + size.y/2});
        }

        Rectangle getRec() { return rec;}

        friend class Lobby;

};

class Lobby {
    private:
        Texture2D background;
        Texture2D menuIcon;
        Cap* player;
        Room* rooms[LOBBY_ROWS * LOBBY_COLS];
        RoomState map[LOBBY_ROWS][LOBBY_COLS] {
            { CUSTOMIZE , EMPTY , EMPTY, EMPTY, EMPTY, EMPTY, EARN},
            { EMPTY, EMPTY, SPACESHOOTER, CAR, PONG, EMPTY, EMPTY},
            { EAT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WITHDRAW}
        };
        RoomState currentState;
        string roomName[8] = {
            "EMPTY", "CUSTOMIZE", "PONG", "WITHDRAW", "CAR GAME", "SPACESHOOTER", "EARN", "EAT"
        };
            
        MessageManager* messages;
        bool energyWarning = false;
        Font font = LoadFontEx("assets/fonts/Montserrat-SemiBold.ttf", TOKEN_FONT_SIZE, NULL, 0);
    public:
        Lobby (Cap* p, MessageManager* m) : player(p), messages(m) {
            background = LoadTexture("assets/lobby/background.png");
            menuIcon = LoadTexture("assets/lobby/menu.png");

            for (int i = 0; i < LOBBY_ROWS * LOBBY_COLS; i++) {
                rooms[i] = nullptr;
            }

            float height = WINDOW_HEIGHT/LOBBY_ROWS;
            float width = WINDOW_WIDTH/LOBBY_COLS;
            
            Vector2 currentPos = {0,0};
            for(int i = 0; i < LOBBY_ROWS; i++) {
                currentPos.y = height * i;
                for(int j = 0; j < LOBBY_COLS; j++) {
                    currentPos.x = width * j;
                    rooms[i * LOBBY_COLS + j] = new Room(map[i][j], currentPos, {width,height});
                }
            }

            currentState = EMPTY;

            // int fontSize = 30;
            // Font font = LoadFontEx("assets/fonts/Oswald-Bold.ttf", 30, NULL, 0);
        }
        
        ~Lobby () {
            // for(int i = 0; i < LOBBY_ROWS * LOBBY_COLS; i++) {
            //     delete rooms[i];
            // }
            UnloadTexture(background);
            UnloadTexture(menuIcon);
        }

        void draw () {
            DrawTexture(background, 0, 0, WHITE);

            int menuX = GetScreenWidth() - menuIcon.width - 20;
            int menuY = GetScreenHeight() / 2 - menuIcon.height / 2;
            DrawTexture(menuIcon, menuX, menuY, WHITE);

            string text = "Available Tokens: " + to_string(player->getTokens());
            Vector2 textSize = MeasureTextEx(font, text.c_str(), TOKEN_FONT_SIZE, 0);
            DrawTextEx(font, (text).c_str(), {WINDOW_WIDTH/2 - textSize.x/2, 100 - textSize.y/2}, TOKEN_FONT_SIZE, 0, TEXT_COLOR);

            float barWidth = 300;
            float barHeight = 25;
            float energyPercent = player->getEnergy() / 100.0f;
            float x = WINDOW_WIDTH / 2 - barWidth / 2;
            float y = 120 + textSize.y + 10;
            string label = "Consumable Energy";
            Vector2 labelSize = MeasureTextEx(font, label.c_str(), TOKEN_FONT_SIZE/1.5, 0);
            DrawTextEx(font, label.c_str(), {WINDOW_WIDTH / 2 - labelSize.x / 2, y - TOKEN_FONT_SIZE/2 - 10}, TOKEN_FONT_SIZE/1.5, 0, Color{0x00, 0x47, 0x65, 255});
            DrawRectangleRounded({x, y, barWidth, barHeight}, 0.5f, 10, Color{200, 220, 230, 255}); 
            DrawRectangleRounded({x, y, barWidth * energyPercent, barHeight}, 0.5f, 10, Color{0x00, 0x47, 0x65, 255});

            for(int i = 0; i < LOBBY_ROWS * LOBBY_COLS; i++) {
                rooms[i]->drawRoom();
            }
            player->draw();
            

<<<<<<< HEAD
                // text = "Press ENTERKEY to Enter City";
                // textSize = MeasureTextEx(font, text.c_str(), 20, 0);
                // DrawTextEx(font, text.c_str(), {(WINDOW_WIDTH/LOBBY_COLS)/2 - textSize.x/2, WINDOW_HEIGHT/2 - textSize.y/2 + 15}, fontSize, 0, {0, 71, 101, 255});

            }

            Rectangle menuRect = { (float)menuX, (float)menuY, (float)menuIcon.width, (float)menuIcon.height };
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, menuRect)) {
                    // Event
                }
            }
=======
            checkContains();
>>>>>>> 99e534d80f5bcab2329297a6c9afb59355205c27
        }
        
        bool CheckRectangleContainsRec(Rectangle outer, Rectangle inner) {
            return (inner.x >= outer.x && inner.y >= outer.y && inner.x + inner.width <= outer.x + outer.width && inner.y + inner.height <= outer.y + outer.height);
        }
        
        bool checkContains() {
            Rectangle playerRec = player->getRectangle();
            for(int i = 0; i < LOBBY_ROWS * LOBBY_COLS; i++) {
                if (rooms[i]->gameState != EMPTY &&CheckRectangleContainsRec(rooms[i]->getRec(), playerRec)) {
                    currentState = rooms[i]->gameState;
                    return true;
                }
            }
            currentState = EMPTY;
            return false;
        }

        int update() {
            float dt = GetFrameTime();
            player->update(dt);
            if(player->getEnergy() < 20 && !energyWarning) {
                messages->addMessage("You're low on energy! Eat some fruit.");
                energyWarning = true;
            }
            if(currentState != EMPTY && IsKeyPressed(KEY_ENTER)) {
                bool changeState = true;
                switch (currentState) {
                    case SPACESHOOTER:
                        if(player->getTokens() >= SPACESHOOTER_TOKENS) {
                            player->removeTokens(SPACESHOOTER_TOKENS);
                        } else {
                            changeState = false;
                            messages->addMessage("You need atleast " + to_string(SPACESHOOTER_TOKENS) + " tokens to enter Space City!");
                        }
                        break;
                    case PONG:
                        if(player->getTokens() >= PONG_TOKENS) {
                            player->removeTokens(PONG_TOKENS);
                        } else {
                            changeState = false;
                            messages->addMessage("You need atleast " + to_string(PONG_TOKENS) + " tokens to enter Pong City!");
                        }
                        break;
                    case CAR:
                        if(player->getTokens() >= CAR_TOKENS) {
                            player->removeTokens(CAR_TOKENS);
                        } else {
                            changeState = false;
                            messages->addMessage("You need atleast " + to_string(CAR_TOKENS) + " tokens to enter Car City!");
                        }
                        break;
                    case EAT:
                        energyWarning = false;
                        break;
                    default:
                        break;
                }

                if(changeState) {
                    return currentState;
                }
            } 
            return EMPTY;
        }

};

#endif
