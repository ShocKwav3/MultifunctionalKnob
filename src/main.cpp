#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "AiEsp32RotaryEncoder.h"
#include "BleKeyboard.h"

#define ROTARY_ENCODER_A_PIN 1
#define ROTARY_ENCODER_B_PIN 0
#define ROTARY_ENCODER_BUTTON_PIN 2
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 6
#define SCL_PIN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

BleKeyboard bleKeyboard("KnobKoKy", "KoKy", 69);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

unsigned long shortPressAfterMiliseconds = 50;
unsigned long longPressAfterMiliseconds = 1000;

void on_button_short_click()
{
    Serial.print("button SHORT press ");
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

void on_button_long_click()
{
    Serial.print("button LONG press ");
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

void handle_rotary_button()
{
    static unsigned long lastTimeButtonDown = 0;
    static bool wasButtonDown = false;

    bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
    // isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

    if (isEncoderButtonDown)
    {
        Serial.print("+"); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
        if (!wasButtonDown)
        {
            // start measuring
            lastTimeButtonDown = millis();
        }
        // else we wait since button is still down
        wasButtonDown = true;
        return;
    }

    // button is up
    if (wasButtonDown)
    {
        Serial.println(""); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
        // click happened, lets see if it was short click, long click or just too short
        if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)
        {
            on_button_long_click();
        }
        else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds)
        {
            on_button_short_click();
        }
    }
    wasButtonDown = false;
}

void rotary_loop()
{
    // dont print anything unless value changed
    if (rotaryEncoder.encoderChanged())
    {
        Serial.print("Value: ");
        Serial.println(rotaryEncoder.readEncoder());
    }
    handle_rotary_button();
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void setup()
{
    Serial.begin(460800);
    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    bleKeyboard.begin();

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, 1000, circleValues);
    rotaryEncoder.setAcceleration(250);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 5);
    display.println("Knob firmware running!");
    display.display();
}

void loop()
{
    rotary_loop();

    if (bleKeyboard.isConnected())
    {
        delay(1000);

        // Serial.println("Sending Play/Pause media key...");
        // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

        Serial.println("Scrolling left...");
        bleKeyboard.mouseMove(0, 0, 0, -3); // horizontal scrool

        delay(1000);

        Serial.println("Scrolling right...");
        bleKeyboard.mouseMove(0, 0, 0, 3); // horizontal scrool

        delay(1000);

        Serial.println("Scrolling down...");
        bleKeyboard.mouseMove(0, 0, -3, 0); // vertical scrool down

        delay(1000);

        Serial.println("Scrolling up...");
        bleKeyboard.mouseMove(0, 0, 3, 0); // vertical scrool up
    }

    Serial.println("Waiting 3 seconds...");
    delay(3000);
}
