#include <Arduino.h>
#include <string>
#include <vector>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_Keypad.h"

#define OLED_RESET D13

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define CHAR_BASE_W 6
#define CHAR_BASE_H 8
#define FONT_SIZE 2

#define CHAR_W (CHAR_BASE_W * FONT_SIZE)
#define CHAR_H (CHAR_BASE_H * FONT_SIZE)

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 4; // rows
const byte COLS = 4; // columns

char keys[ROWS][COLS] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '+'},
    {'0', 'C', 'E', '-'}};
byte rowPins[ROWS] = {11, 10, 9, 8}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4};   // connect to the column pinouts of the keypad

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

std::vector<std::string> buffer;

int cursorX = 0, cursorY = 0;

void setup()
{
    Serial.begin(115200);
    customKeypad.begin();

    display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // initialize with the I2C addr 0x3D (for the 128x64)
    display.clearDisplay();
    display.display();

    // text display tests
    display.setTextSize(FONT_SIZE);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    buffer.push_back("");
}

void loop()
{
    customKeypad.tick();

    while (customKeypad.available())
    {
        keypadEvent e = customKeypad.read();
        // Serial.print((char)e.bit.KEY);

        if (e.bit.EVENT == KEY_JUST_RELEASED)
        {
            if (e.bit.KEY == 'E')
            {
                if (buffer[cursorY] == "")
                {
                    continue;
                }

                display.println();
                cursorY++;
                cursorX = 0;
                buffer.push_back("");

                display.display();
            }
            else if (e.bit.KEY == 'C')
            {
                display.fillRect(0, cursorY * CHAR_H, DISPLAY_WIDTH, CHAR_H, BLACK);
                display.setCursor(0, cursorY * CHAR_H);

                cursorX = 0;
                buffer[cursorY] = "";

                display.display();
            }
            else if (e.bit.KEY == '+')
            {
                if (cursorY < 1)
                    continue;

                if (buffer[cursorY] == "" && cursorY > 1)
                    cursorY--;

                display.fillRect(0, (cursorY - 1) * CHAR_H, DISPLAY_WIDTH, CHAR_BASE_H * FONT_SIZE * 2, BLACK);
                display.setCursor(0, (cursorY - 1) * CHAR_H);

                long int operandA = atol(buffer[cursorY - 1].c_str());
                long int operandB = atol(buffer[cursorY].c_str());
                long int ans = operandA + operandB;

                Serial.println(operandA);
                Serial.println(operandB);

                cursorX = 0;

                display.println(operandA + operandB);
                buffer[cursorY - 1] = std::to_string(ans);

                buffer[cursorY] = "";

                display.display();
            }
            else
            {
                display.print(static_cast<char>(e.bit.KEY));

                buffer[cursorY] += static_cast<char>(e.bit.KEY);
                cursorX++;
                display.display();
            }
        }
    }

    delay(10);
}
