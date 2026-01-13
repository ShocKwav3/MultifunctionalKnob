#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "EncoderDriver.h"
#include "BleKeyboard.h"
#include <Preferences.h>

#include "Config/device_config.h"
#include "Config/encoder_config.h"
#include "Config/log_config.h"
#include "Config/FactoryReset.h"
#include "Config/ConfigManager.h"
#include "Display/DisplayFactory.h"
#include "Helper/EncoderModeHelper.h"
#include "Enum/WheelModeEnum.h"
#include "Type/EncoderInputEvent.h"
#include "Type/ButtonEvent.h"
#include "Type/AppEvent.h"
#include "Enum/EventEnum.h"
#include "Event/Dispatcher/EncoderEventDispatcher.h"
#include "Event/Handler/EncoderEventHandler.h"
#include "Event/Dispatcher/AppEventDispatcher.h"
#include "Event/Handler/AppEventHandler.h"
#include "Event/Dispatcher/ButtonEventDispatcher.h"
#include "Event/Handler/ButtonEventHandler.h"
#include "EncoderMode/Handler/EncoderModeHandlerScroll.h"
#include "EncoderMode/Handler/EncoderModeHandlerVolume.h"
#include "EncoderMode/Handler/EncoderModeHandlerZoom.h"
#include "EncoderMode/Selector/EncoderModeSelector.h"
#include "EncoderMode/Manager/EncoderModeManager.h"
#include "Menu/Controller/MenuController.h"
#include "Menu/Model/MenuTree.h"
#include "Menu/Action/ShowStatusAction.h"
#include "Menu/Action/ShowAboutAction.h"
#include "Button/ButtonManager.h"
#include "Display/Task/DisplayTask.h"
#include "Display/Model/DisplayRequest.h"
#include "Event/Handler/MenuEventHandler.h"
#include "Event/Dispatcher/MenuEventDispatcher.h"
#include "Type/MenuEvent.h"
#include "AppState.h"
#include "BLE/BleCallbackHandler.h"

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

    // Register BLE connection state callbacks for user feedback
    bleKeyboard.setOnConnect([]() {
        BleCallbackHandler::handleConnect(&appState.blePairingState, appState.displayRequestQueue);
    });

    bleKeyboard.setOnDisconnect([](int reason) {
        BleCallbackHandler::handleDisconnect(reason, &appState.blePairingState,
                                            appState.displayRequestQueue, &bleKeyboard);
    });

    appState.encoderInputEventQueue = xQueueCreate(10, sizeof(EncoderInputEvent));
    appState.buttonEventQueue = xQueueCreate(10, sizeof(ButtonEvent));
    appState.appEventQueue = xQueueCreate(10, sizeof(AppEvent));
    appState.menuEventQueue = xQueueCreate(10, sizeof(MenuEvent));
    appState.displayRequestQueue = xQueueCreate(10, sizeof(DisplayRequest));

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

    // Initialize button event system
    static ButtonEventDispatcher buttonEventDispatcher(appState.buttonEventQueue);
    static ButtonEventHandler buttonEventHandler(appState.buttonEventQueue, &configManager, &bleKeyboard);
    buttonEventHandler.start();

    // Initialize display pipeline
    static DisplayTask displayTask(&DisplayFactory::getDisplay());
    appState.displayRequestQueue = displayTask.init(10);
    displayTask.start(2048, 1);

    // Initialize menu event pipeline
    MenuEventDispatcher::init(appState.menuEventQueue);
    static MenuEventHandler menuEventHandler(appState.menuEventQueue, appState.displayRequestQueue);
    menuEventHandler.start(2048, 1);

    // Initialize menu system
    static MenuController menuController;
    MenuTree::initMenuTree();
    MenuTree::initWheelBehaviorActions(&configManager, &encoderModeManager);
    MenuTree::initButtonBehaviorActions(&configManager, &buttonEventHandler);
    MenuTree::initBluetoothActions(&bleKeyboard, appState.displayRequestQueue, &appState.blePairingState);
    
    // Initialize Device Status and About actions
    static ShowStatusAction showStatusAction(&configManager, &bleKeyboard, &DisplayFactory::getDisplay());
    static ShowAboutAction showAboutAction(&DisplayFactory::getDisplay());
    MenuTree::setDeviceStatusAction(&showStatusAction);
    MenuTree::setAboutAction(&showAboutAction);
    
    encoderEventHandler.setMenuController(&menuController);

    static AppEventHandler appEventHandler(appState.appEventQueue, &encoderModeManager);
    appEventHandler.start();

    static ButtonManager buttonManagerInstance(&buttonEventDispatcher);
    buttonManagerInstance.init();

    static EncoderEventDispatcher encoderEventDispatcher(appState.encoderInputEventQueue, &configManager);
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
    // Button polling now handled by dedicated FreeRTOS task in ButtonManager (Story 6.3)
    // Empty loop - all input handling occurs in event-driven tasks
}
