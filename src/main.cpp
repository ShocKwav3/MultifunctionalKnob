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
#include "state/AppState.h"
#include "state/HardwareState.h"
#include "BLE/BleCallbackHandler.h"

BleKeyboard bleKeyboard(BLUETOOTH_DEVICE_NAME, BLUETOOTH_DEVICE_MANUFACTURER, BLUETOOTH_DEVICE_BATTERY_LEVEL_DEFAULT);
EncoderDriver* encoderDriver;
AppState appState;
HardwareState hardwareState;  // Global hardware state instance

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

    bleKeyboard.begin();

    // Register BLE connection state callbacks for user feedback
    bleKeyboard.setOnConnect([]() {
        BleCallbackHandler::handleConnect(appState.displayRequestQueue);
    });

    bleKeyboard.setOnDisconnect([](int reason) {
        BleCallbackHandler::handleDisconnect(reason, appState.displayRequestQueue, &bleKeyboard);
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

    static EncoderModeManager encoderModeManager(&encoderEventHandler, &encoderModeSelector, appState.displayRequestQueue, &hardwareState);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL, &encoderModeHandlerScroll);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME, &encoderModeHandlerVolume);
    encoderModeManager.registerHandler(EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM, &encoderModeHandlerZoom);

    // Load saved wheel mode from NVS and apply (defaults to SCROLL if no config exists)
    WheelMode savedWheelMode = configManager.loadWheelMode();
    EventEnum::EncoderModeEventTypes initialMode = EncoderModeHelper::fromWheelMode(savedWheelMode);
    encoderModeManager.setMode(initialMode);

    // Initialize hardware state with loaded config
    hardwareState.encoderWheelState.mode = savedWheelMode;
    hardwareState.encoderWheelState.direction = configManager.getWheelDirection();
    // TODO: Implement battery monitoring via ADC + voltage divider on GPIO pin
    // Current placeholder value (100%) should be replaced with periodic ADC reads
    // and voltage-to-percentage conversion. Consider adding low-battery warning threshold.
    hardwareState.batteryPercent = 100;
    hardwareState.bleState.isConnected = false;  // Updated by BLE callbacks
    hardwareState.bleState.isPairingMode = false;
    hardwareState.displayPower = true;  // Display always starts ON after boot (session-only toggle)

    // Initialize button event system
    static ButtonEventDispatcher buttonEventDispatcher(appState.buttonEventQueue);
    static ButtonEventHandler buttonEventHandler(appState.buttonEventQueue, &configManager, &bleKeyboard);
    buttonEventHandler.start();

    // Initialize display pipeline
    static DisplayTask displayTask(&DisplayFactory::getDisplay());
    appState.displayRequestQueue = displayTask.init(10);
    displayTask.start(2048, 1);

    // Initialize display power state (always ON at boot for visual feedback)
    DisplayFactory::getDisplay().setPower(hardwareState.displayPower);
    LOG_INFO("Main", "Display power initialized: %s", hardwareState.displayPower ? "ON" : "OFF");

    // Create BT flash timer for pairing animation (500ms period = 1Hz blink rate)
    // Timer is started/stopped dynamically when entering/exiting pairing mode
    appState.btFlashTimer = xTimerCreate(
        "BTFlash",              // Timer name
        pdMS_TO_TICKS(500),     // 500ms period
        pdTRUE,                 // Auto-reload (periodic)
        nullptr,                // Timer ID (unused)
        BleCallbackHandler::btFlashTimerCallback    // Callback function
    );

    // Show welcome message on boot
    DisplayRequest initRequest;
    initRequest.type = DisplayRequestType::SHOW_MESSAGE;
    initRequest.data.message.value = "Ready";
    xQueueSend(appState.displayRequestQueue, &initRequest, portMAX_DELAY);

    // Wait briefly for welcome message, then show normal mode status
    vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second delay
    DisplayRequest normalModeRequest;
    normalModeRequest.type = DisplayRequestType::DRAW_NORMAL_MODE;
    normalModeRequest.data.normalMode.hwState = hardwareState;
    xQueueSend(appState.displayRequestQueue, &normalModeRequest, portMAX_DELAY);

    // Initialize menu event pipeline
    MenuEventDispatcher::init(appState.menuEventQueue);
    static MenuEventHandler menuEventHandler(appState.menuEventQueue, appState.displayRequestQueue, &hardwareState);
    menuEventHandler.start(2048, 1);

    // Initialize menu system
    static MenuController menuController;
    MenuTree::initMenuTree();
    MenuTree::initWheelBehaviorActions(&configManager, &encoderModeManager, appState.displayRequestQueue, &hardwareState);
    MenuTree::initButtonBehaviorActions(&configManager, &buttonEventHandler);
    MenuTree::initBluetoothActions(&bleKeyboard, appState.displayRequestQueue);
    MenuTree::initDisplayActions(&DisplayFactory::getDisplay(), &menuController);

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
}

void loop()
{
    // Button polling now handled by dedicated FreeRTOS task in ButtonManager (Story 6.3)
    // Empty loop - all input handling occurs in event-driven tasks
}
