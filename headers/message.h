#ifndef MESSAGE_H
#define MESSAGE_H

#include "C:\raylib\raylib\src\raylib.h"
#include "custom_timer.h"
#include <iostream>
using namespace std;

class Message {
    private:
        string text;
        int size;
        Timer message_timer;
        bool visible;
    public:
        Message(string txt, int duration = 5, int fontSize = 20) : text(txt), size(fontSize), message_timer(duration, false, true, [&]() {visible = false; }), visible(true) {}

        void update() {
            message_timer.update();
        }

        void draw(Vector2 pos, Font font, Color color = Color{0x00, 0x47, 0x65, 255}, Color bgColor = Color{0xff, 0xfd, 0xeb, 125}) {
            if (!visible) {
                return;
            }
        
            int padding = 16;
            float spacing = 1;
        
            Vector2 textSize = MeasureTextEx(font, text.c_str(), size, spacing);
            float bgWidth = textSize.x + 2 * padding;
            float bgHeight = textSize.y + 2 * padding;
        
            Rectangle bg = { pos.x - padding, pos.y - padding, bgWidth, bgHeight };
        
            DrawRectangleRounded(bg, 0.4, 16, bgColor);
            DrawRectangleRoundedLines(bg, 0.4, 16, color);
            DrawTextEx(font, text.c_str(), pos, size, spacing, color);
        }        

        bool isVisible() const {
            return visible;
        }
    
        float getHeight(Font font) const {
            return MeasureTextEx(font, text.c_str(), size, 1).y + 20;
        }
 
};

class MessageManager {
    private:
        Message** messages;
        int numMessages;
        Vector2 startPos = {20, 20};
        Font font;
    public:
        MessageManager(Font f) : messages(nullptr), numMessages(0), font(f) {}

        void addMessage(string text) {
            Message** tempMessages = new Message*[numMessages + 1];
            for(int i = 0; i < numMessages; i++) {
                tempMessages[i] = messages[i];
            }
            tempMessages[numMessages] = new Message(text);
            delete[] messages;
            messages = tempMessages;
            numMessages++;
        }

        void removeMessage(int index) {
            if (index < 0 || index >= numMessages) {
                return;
            }
            delete messages[index];
            Message** tempMessages = new Message*[numMessages - 1];
            for (int i = 0, j = 0; i < numMessages; i++) {
                if (i != index) {
                    tempMessages[j++] = messages[i];
                }
            }
            delete[] messages;
            messages = tempMessages;
            numMessages--;
        }        

        void update() {
            for (int i = 0; i < numMessages; i++) {
                if(!messages[i]->isVisible()) {
                    removeMessage(i);
                } else {
                    messages[i]->update();
                }
            }
        }

        void draw() {
            Vector2 currentPos = startPos;
            for(int i = 0; i < numMessages; i++) {
                if(messages[i]->isVisible()) {
                    messages[i]->draw(currentPos, font);
                    currentPos.y += messages[i]->getHeight(font) + 20;
                }
            }
        }

        ~MessageManager() {
            for(int i = 0; i < numMessages; i++) {
                delete messages[i];
            }
            delete[] messages;
        }
};


#endif
