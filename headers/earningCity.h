#ifndef EARN_H
#define EARN_H

#include "C:\raylib\raylib\src\raylib.h"
#include "settings.h"
#include "cap.h"
#include <iostream>
#include <fstream>
using namespace std;

class Question {
    private:
        string question;
        string options[4];
        int correctOption;
        int cashReward;
        int penalty;
    public:
        // Question() : question(""), correctOption(0), cashReward(0), penalty(0) {
        //     for (int i = 0; i < 4; i++) {
        //         options[i] = "";
        //     }
        // }

        Question(string q, string opts[4], int cO, int cR, int p) : question(q), correctOption(cO), cashReward(cR), penalty(p) {
            for (int i = 0; i < 4; i++) {
                options[i] = opts[i];
            }
        } 
        string getQuestion() {
            return question;
        }
        int getCorrectOption() {
            return correctOption;
        }
        int getCashReward() {
            return cashReward;
        }
        int getPenalty() {
            return penalty;
        }
        string getOption(int i) {
            if(i < 4 && i > -1) {
                return options[i];
            }
            return "";
        }
        bool answer(int c) {
            return c == correctOption;
        }

};

class EarnCity {

    private:
        Cap* player;
        Question** questions;
        Question* currentQuestion;
        int numQuestions;
        Font font;
        enum EarnState {
            WELCOME,
            CORRECT,
            INCORRECT,
            ANSWER
        };
        EarnState currentState;
        Texture2D welcome;
        Texture2D correct;
        Texture2D incorrect;
        Texture2D answer;
        int streak;
        string streakComments[10] = { "On a roll!", "Streak mode!", "Unstoppable!", "Can't miss!", "Hot streak!", "Fire run!", "Winning streak!", "You're blazing!", "Keep the streak!", "Chain reaction!" };

    public:
        EarnCity(Cap* p) : player(p), currentState(WELCOME), streak(0){

            welcome = LoadTexture("assets/earnCity/welcome.png");
            correct = LoadTexture("assets/earnCity/correct.png");
            incorrect = LoadTexture("assets/earnCity/incorrect.png");
            answer = LoadTexture("assets/earnCity/answer.png");
            

            ifstream file("assets/earnCity/earn_questions.csv");
            if (!file.is_open()) {
                cout << "Error opening file earn_questions.csv!" << endl;
                return;
            }
            string line;
            getline(file, line);

            int lines = 0;
            while(getline(file, line)) {
                lines ++;
            }

            numQuestions = lines;
            questions = new Question*[numQuestions];

            file.clear();
            file.seekg(0);

            getline(file,line);

            lines = 0;
            while(getline(file, line) && lines < numQuestions) {
                string fields[8];
                int i = 0;
                string temp = "";

                for(int j = 0; j < (int)line.length(); j++) {
                    if (line[j] == ',') {
                        fields[i++] = temp;
                        temp = "";
                    } else {
                        temp += line[j];
                    }
                }
                fields[i] = temp;

                string question = fields[0];
                string options[4] = {fields[1], fields[2], fields[3], fields[4]};
                int correctOption = stoi(fields[5]);
                int cashReward = stoi(fields[6]);
                int penalty = stoi(fields[7]);

                questions[lines++] = new Question(question, options, correctOption, cashReward, penalty); 
            }

            file.close();
            font = LoadFontEx("assets/fonts/Montserrat-SemiBold.ttf", EARN_FONT_SIZE, nullptr, 0);
        }

        ~EarnCity () {
            UnloadTexture(welcome);
            UnloadTexture(correct);
            UnloadTexture(incorrect);
            UnloadTexture(answer);
        }

        void draw() {

            bool updated = false;

            if(currentState == WELCOME) {
                DrawTexture(welcome, 0, 0, WHITE);
            }

            if(currentState == CORRECT) {
                DrawTexture(correct, 0, 0, WHITE);
                string text = "Deposited $" + to_string(currentQuestion->getCashReward()) + " to your Bank";
                Vector2 textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/4, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x / 2, WINDOW_HEIGHT - textSize.y / 2 - 120}, EARN_FONT_SIZE/4, 0, EARN_WHITE);
                updated = true;
                if(streak > 2) {
                    text = streakComments[streak % 10] + " Streak: " + to_string(streak);
                    textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/2, 0);
                    DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x / 2, 250 - textSize.y/2}, EARN_FONT_SIZE/2, 0, {255, 254, 235, 255});
                }
            }

            if(currentState == INCORRECT) {
                DrawTexture(incorrect, 0, 0, EARN_WHITE);
                string text = "";
                if(player->getCash() != 0) {
                    if(player->getCash() < currentQuestion->getPenalty()) {
                        text = "Insufficient Cash, Partial penalty of $" + to_string(player->getCash()) + " imposed!";
                    } else {
                        text = "Deducted $" + to_string(currentQuestion->getCashReward()) + " from your Bank";
                    }
                    updated = true;
                } else {
                    text = "Out of Cash? No penalty for you poor friend!";
                }

                Vector2 textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/4, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x / 2, WINDOW_HEIGHT - textSize.y / 2 - 120}, EARN_FONT_SIZE/4, 0, EARN_WHITE);
                    
            }

            if(updated) {
                string text = "Check ATM for updated balance";
                Vector2 textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/4, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x / 2, WINDOW_HEIGHT - textSize.y / 2 - 80}, EARN_FONT_SIZE/4, 0, EARN_WHITE);
            }

            if(currentState == ANSWER) {
                DrawTexture(answer, 0, 0, WHITE);

                string text = "ANSWER THE FOLLOWING TO GET CASH REWARD!";
                Vector2 textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/2, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x / 2, 200 - textSize.y / 2}, EARN_FONT_SIZE/2, 0, EARN_WHITE);

                text = currentQuestion->getQuestion();
                textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x/2, WINDOW_HEIGHT/2 - textSize.y/2 - 200}, EARN_FONT_SIZE, 0, EARN_WHITE);

                const float optionWidth = 400.0f;
                const float optionHeight = 60.0f;
                const float optionPadding = 20.0f;

                for (int i = 0; i < 4; i++) {
                    Vector2 pos;
                    text = string(1, 'A' + i) + ". " + currentQuestion->getOption(i);
                    textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/3, 0);

                    if (i < 2) {
                        pos.x = WINDOW_WIDTH/4 - optionWidth/2;
                        pos.y = WINDOW_HEIGHT/2 + (i * (optionHeight + optionPadding));
                    } else {
                        pos.x = 3*WINDOW_WIDTH/4 - optionWidth/2;
                        pos.y = WINDOW_HEIGHT/2 + ((i - 2) * (optionHeight + optionPadding));
                    }
                    Color rectColor = {255, 253, 235, 64};
                    DrawRectangleRounded(
                        (Rectangle){pos.x, pos.y, optionWidth, optionHeight},
                        0.3f, 10, rectColor
                    );
                    Vector2 textPos;
                    textPos.x = pos.x + (optionWidth - textSize.x) / 2;
                    textPos.y = pos.y + (optionHeight - textSize.y) / 2;

                    DrawTextEx(font, text.c_str(), textPos, EARN_FONT_SIZE/3, 0, WHITE);
                }

                
                text = "Cash Reward: " + to_string(currentQuestion->getCashReward());
                textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/4, 0);
                DrawTextEx(font, text.c_str(), {200 - textSize.x/2, 100 - textSize.y/2}, EARN_FONT_SIZE/4, 0, EARN_WHITE);

                text = "Penalty: " + to_string(currentQuestion->getPenalty());
                textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/4, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH - textSize.x/2 - 200, 100 - textSize.y/2}, EARN_FONT_SIZE/4, 0, EARN_WHITE);

                text = "Press S to SKIP";
                textSize = MeasureTextEx(font, text.c_str(), EARN_FONT_SIZE/2.5, 0);
                DrawTextEx(font, text.c_str(), {WINDOW_WIDTH/2 - textSize.x/2, WINDOW_HEIGHT - 100 - textSize.y/2}, EARN_FONT_SIZE/2.5, 0, EARN_WHITE);
            }

        }

        void reset() {
            streak = 0;
            currentState = WELCOME;
        }

        void updateQuestion() {
            currentQuestion = questions[rand() % numQuestions];
        }

        void update() {

            if(currentState == WELCOME && IsKeyPressed(KEY_E)) {
                currentState = ANSWER;
                updateQuestion();
            }

            if(currentState == ANSWER) {

                int answer = -1;

                if(IsKeyPressed(KEY_A)) {
                    answer = 0;
                } else if(IsKeyPressed(KEY_B)) {
                    answer = 1;
                } else if(IsKeyPressed(KEY_C)) {
                    answer = 2;
                } else if(IsKeyPressed(KEY_D)) {
                    answer = 3;
                } else if(IsKeyPressed(KEY_S)) {
                    answer = 4;
                }

                if (answer != -1) {
                    if (answer == currentQuestion->getCorrectOption()) {
                        player->addCash(currentQuestion->getCashReward());
                        currentState = CORRECT;
                        streak++;
                    } else if (answer == 4) {
                        streak = 0;
                        updateQuestion();
                    } else {
                        streak = 0;
                        player->removeCash((currentQuestion->getPenalty() > player->getCash()) ? player->getCash() : currentQuestion->getPenalty());
                        currentState = INCORRECT;
                    }
                }

            }

            if(currentState == CORRECT || currentState == INCORRECT) {
                if(IsKeyPressed(KEY_E)) {
                    updateQuestion();
                    currentState = ANSWER;
                }
            }

        }

};


#endif

