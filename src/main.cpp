#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "EncoderDriver.h"
#include "BleKeyboard.h"

#include "Config/device_config.h"
#include "Config/encoder_config.h"
#include "Type/EncoderInputEvent.h"
#include "Type/AppEvent.h"
#include "Enum/EventEnum.h"
#include "Event/Dispatcher/EncoderInputEventDispatcher.h"
#include "Event/Handler/EncoderInputEventHandler.h"
#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Event/Handler/AppEventHandler.h"
#include "EncoderMode/Handler/EncoderModeHandlerScroll.h"
#include "EncoderMode/Handler/EncoderModeHandlerVolume.h"
#include "AppState.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 6
#define SCL_PIN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BleKeyboard bleKeyboard(BLUETOOTH_DEVICE_NAME, BLUETOOTH_DEVICE_MANUFACTURER, BLUETOOTH_DEVICE_BATTERY_LEVEL_DEFAULT);
EncoderDriver* encoderDriver;
AppState appState;

void setup()
{
    Serial.begin(460800);
    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    bleKeyboard.begin();

    appState.encoderInputEventQueue = xQueueCreate(10, sizeof(EncoderInputEvent));
    appState.appEventQueue = xQueueCreate(10, sizeof(AppEvent));

    static AppEventDispatcher appDispatcher(appState.appEventQueue);
    static EncoderModeHandlerScroll encoderModeHandlerScroll(&appDispatcher);
    static EncoderModeHandlerVolume encoderModeHandlerVolume(&appDispatcher);

    static EncoderInputEventHandler encoderInputEventHandler(appState.encoderInputEventQueue);
    encoderInputEventHandler.start();

    static AppEventHandler appEventHandler(appState.appEventQueue);
    appEventHandler.start();

    static EncoderInputEventDispatcher encoderInputEventDispatcher(appState.encoderInputEventQueue);
    encoderDriver = EncoderDriver::getInstance(
        ENCODER_PIN_A,
        ENCODER_PIN_B,
        ENCODER_PIN_BUTTON,
        ENCODER_PIN_VCC,
        ENCODER_STEPS
    );
    encoderDriver->setOnValueChange([](int32_t newValue) {
        encoderInputEventDispatcher.onEncoderValueChange(newValue);
    });

    encoderDriver->setOnShortClick([]() {
        encoderInputEventDispatcher.onShortClick();
    });

    encoderDriver->setOnLongClick([]() {
        encoderInputEventDispatcher.onLongClick();
    });
    encoderDriver->begin();

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

    Serial.println("Waiting 5 seconds...");
    delay(5000);
}
