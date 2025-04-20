#ifndef ATMCITY_H
#define ATMCITY_H

#include "C:\raylib\raylib\src\raylib.h"
#include "cap.h"
#include "settings.h"

class ATMCity {
    private:
        int atmBalance;
        int tokensCount;

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

        ATMCity () {
            atmBalance = 1000; // initial balance
            tokensCount = 0;
            atmState = MACHINE;

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
                DrawText(TextFormat("%i", atmBalance), WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2 - 100, 300, DARKBROWN);
            }

            if (atmState == WITHDRAW){
                DrawTexture (withdraw, 0, 0, WHITE);
                DrawText(TextFormat("%i", tokensCount), WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 2 - 150, 300, DARKBROWN);
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
                tokensCount += (atmBalance / 50);
                atmBalance = 0;

                if (IsKeyPressed(KEY_R)) {
                    atmState = MACHINE;
                }
            }
        }

        void setBalance (int b) {
            atmBalance = b;
        }

        void setTokensCount (int t) {
            tokensCount = t;
        }

        int getBalance () {
            return atmBalance; 
        }

        int getTokensCount () {
            return tokensCount; 
        }
};

#endif