#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "EncoderDriver.h"
#include "BleKeyboard.h"
#include <Preferences.h>

#include "Config/device_config.h"
#include "Config/encoder_config.h"
#include "Config/FactoryReset.h"
#include "Config/ConfigManager.h"
#include "Display/DisplayFactory.h"
#include "Helper/EncoderModeHelper.h"
#include "Enum/WheelModeEnum.h"
#include "Type/EncoderInputEvent.h"
#include "Type/AppEvent.h"
#include "Enum/EventEnum.h"
#include "Event/Dispatcher/EncoderEventDispatcher.h"
#include "Event/Handler/EncoderEventHandler.h"
#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Event/Handler/AppEventHandler.h"
#include "EncoderMode/Handler/EncoderModeHandlerScroll.h"
#include "EncoderMode/Handler/EncoderModeHandlerVolume.h"
#include "EncoderMode/Handler/EncoderModeHandlerZoom.h"
#include "EncoderMode/Selector/EncoderModeSelector.h"
#include "EncoderMode/Manager/EncoderModeManager.h"
#include "Menu/Controller/MenuController.h"
#include "Menu/Model/MenuTree.h"
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

// Configuration management
Preferences preferences;
ConfigManager configManager(&preferences);

void setup()
{
    Serial.begin(460800);

    // Check for factory reset request (button held for 5+ seconds at boot)
    if (FactoryReset::isResetRequested(ENCODER_PIN_BUTTON)) {
        FactoryReset::execute(configManager, DisplayFactory::getDisplay());
    }

    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    bleKeyboard.begin();

    appState.encoderInputEventQueue = xQueueCreate(10, sizeof(EncoderInputEvent));
    appState.appEventQueue = xQueueCreate(10, sizeof(AppEvent));

    static AppEventDispatcher appDispatcher(appState.appEventQueue);
    static EncoderModeHandlerScroll encoderModeHandlerScroll(&appDispatcher, &bleKeyboard);
    static EncoderModeHandlerVolume encoderModeHandlerVolume(&appDispatcher, &bleKeyboard);
    static EncoderModeHandlerZoom encoderModeHandlerZoom(&appDispatcher, &bleKeyboard);
    static EncoderModeSelector encoderModeSelector(&appDispatcher);

    static EncoderEventHandler encoderEventHandler(appState.encoderInputEventQueue);
    encoderEventHandler.start();

    static EncoderModeManager encoderModeManager(&encoderEventHandler, &encoderModeSelector);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL, &encoderModeHandlerScroll);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME, &encoderModeHandlerVolume);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM, &encoderModeHandlerZoom);

    // Load saved wheel mode from NVS and apply (defaults to SCROLL if no config exists)
    WheelMode savedWheelMode = configManager.loadWheelMode();
    EventEnum::EncoderModeEventTypes initialMode = EncoderModeHelper::fromWheelMode(savedWheelMode);
    encoderModeManager.setMode(initialMode);

    // Initialize menu system
    static MenuController menuController;
    MenuTree::initMenuTree();
    MenuTree::initWheelBehaviorActions(&configManager, &encoderModeManager);
    encoderEventHandler.setMenuController(&menuController);

    static AppEventHandler appEventHandler(appState.appEventQueue, &encoderModeManager);
    appEventHandler.start();

    static EncoderEventDispatcher encoderEventDispatcher(appState.encoderInputEventQueue);
    encoderDriver = EncoderDriver::getInstance(
        ENCODER_PIN_A,
        ENCODER_PIN_B,
        ENCODER_PIN_BUTTON,
        ENCODER_PIN_VCC,
        ENCODER_STEPS
    );
    encoderDriver->setOnValueChange([](int32_t newValue) {
        encoderEventDispatcher.onEncoderValueChange(newValue);
    });

    encoderDriver->setOnShortClick([]() {
        encoderEventDispatcher.onShortClick();
    });

    encoderDriver->setOnLongClick([]() {
        encoderEventDispatcher.onLongClick();
    });
    encoderDriver->begin();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 5);
    display.println("Knob firmware! - Concrete implementation");
    display.display();
}

void loop()
{
    Serial.println("Knob firmware running...");

    if (bleKeyboard.isConnected()) {
        Serial.println("BLE connected!");
    }

    delay(5000);
}
