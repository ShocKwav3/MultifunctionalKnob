#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "RotaryEncoderDriver.h"
#include "BleKeyboard.h"

#include "Config/device_config.h"
#include "Config/encoder_config.h"
#include "Type/RotaryEncoderEventType.h"
#include "Event/Dispatcher/RotaryEncoderEventDispatcher.h"
#include "Event/Handler/RotaryEncoderEventHandler.h"
#include "Mode/Handler/ScrollModeHandler.cpp"
#include "AppState.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 6
#define SCL_PIN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BleKeyboard bleKeyboard(BLUETOOTH_DEVICE_NAME, BLUETOOTH_DEVICE_MANUFACTURER, BLUETOOTH_DEVICE_BATTERY_LEVEL_DEFAULT);
RotaryEncoderDriver* rotaryEncoderDriver;
AppState appState;

void setup()
{
    Serial.begin(460800);
    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    bleKeyboard.begin();

    appState.rotaryEventQueue = xQueueCreate(10, sizeof(RotaryEncoderEventType));

    static RotaryEncoderEventDispatcher rotaryEventDispatcher(appState.rotaryEventQueue);

    rotaryEncoderDriver = RotaryEncoderDriver::getInstance(
        ROTARY_ENCODER_A_PIN,
        ROTARY_ENCODER_B_PIN,
        ROTARY_ENCODER_BUTTON_PIN,
        ROTARY_ENCODER_VCC_PIN,
        ROTARY_ENCODER_STEPS
    );
    rotaryEncoderDriver->begin();
    rotaryEncoderDriver->setOnValueChange([](int32_t newValue) {
        rotaryEventDispatcher.onEncoderValueChange(newValue);
    });

    rotaryEncoderDriver->setOnShortClick([]() {
        rotaryEventDispatcher.onShortClick();
    });

    rotaryEncoderDriver->setOnLongClick([]() {
        rotaryEventDispatcher.onLongClick();
    });

    static RotaryEncoderEventHandler rotaryEventHandler(appState.rotaryEventQueue);
    static ScrollModeHandler scrollModeHandler;

    rotaryEventHandler.setModeHandler(&scrollModeHandler);
    rotaryEventHandler.start();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 5);
    display.println("Knob firmware!");
    display.display();
}

void loop()
{
    //rotaryEncoderDriver->runLoop();

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
    delay(1000);
}
