#ifndef ATMCITY_H
#define ATMCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"

class ATMCity {
    private:
        Cap* player;
    public:
        enum ATMState {
            MACHINE,
            WITHDRAW,
            BALANCE
        };

        Texture2D machine;
        Texture2D balance;
        Texture2D withdraw;

        ATMState atmState;

        ATMCity (Cap* p) : player(p), atmState(MACHINE) {

            // Loading Textures
            machine = LoadTexture("assets/atmCity/machine.png");        
            balance = LoadTexture("assets/atmCity/balance.png");        
            withdraw = LoadTexture("assets/atmCity/withdraw.png");        
        }

        ~ATMCity () {
            UnloadTexture(machine);
            UnloadTexture(balance);
            UnloadTexture(withdraw);
        }

        void draw () {
            if (atmState == MACHINE) {
                DrawTexture(machine, 0, 0, WHITE);
            }

            if (atmState == BALANCE) {
                DrawTexture (balance, 0, 0, WHITE);
                DrawText(TextFormat("%i", player->cash), WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2 - 100, 300, DARKBROWN);
            }

            if (atmState == WITHDRAW){
                DrawTexture (withdraw, 0, 0, WHITE);
                DrawText(TextFormat("%i", player->tokens), WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2 - 150, 300, DARKBROWN);
            }
        }

        void update () {
            if (atmState == MACHINE) {
                	if (IsKeyPressed(KEY_B)) {
                        atmState = BALANCE;
                    }
                    if (IsKeyPressed(KEY_W)) {
                        atmState = WITHDRAW;
                    }
            }
            
            if (atmState == BALANCE) {
                if (IsKeyPressed(KEY_R)) {
                    atmState = MACHINE;
                }
            }

            if (atmState == WITHDRAW){
                player->tokens += (player->cash / 50);
                player->cash = 0;

                if (IsKeyPressed(KEY_R)) {
                    atmState = MACHINE;
                }
            }
        }

        // void setBalance (int b) {
        //     player->cash = b;
        // }

        // void setTokensCount (int t) {
        //     player->tokens = t;
        // }

        // int getBalance () {
        //     return player->cash; 
        // }

        // int getTokensCount () {
        //     return player->tokens; 
        // }
};

#endif
