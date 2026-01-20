#include "DisplayTask.h"
#include "Config/log_config.h"

DisplayTask::DisplayTask(DisplayInterface* display)
    : display(display)
    , requestQueue(nullptr)
    , taskHandle(nullptr) {
}

void DisplayTask::init(uint8_t queueSize) {
    requestQueue = xQueueCreate(queueSize, sizeof(DisplayRequest));
    if (requestQueue == nullptr) {
        LOG_ERROR(TAG, "Failed to create request queue");
    }
}

bool DisplayTask::start(uint32_t stackSize, UBaseType_t priority) {
    if (requestQueue == nullptr || display == nullptr) {
        LOG_ERROR(TAG, "Cannot start: queue or display not initialized");
        return false;
    }

    BaseType_t result = xTaskCreate(
        taskFunction,
        "DisplayTask",
        stackSize,
        this,
        priority,
        &taskHandle
    );

    if (result != pdPASS) {
        LOG_ERROR(TAG, "Failed to create task");
        return false;
    }

    LOG_INFO(TAG, "Started");
    return true;
}

QueueHandle_t DisplayTask::getQueue() const {
    return requestQueue;
}

void DisplayTask::taskFunction(void* params) {
    DisplayTask* self = static_cast<DisplayTask*>(params);
    DisplayRequest request;

    while (true) {
        if (xQueueReceive(self->requestQueue, &request, portMAX_DELAY) == pdTRUE) {
            self->processRequest(request);
        }
    }
}

void DisplayTask::processRequest(const DisplayRequest& request) {
    switch (request.type) {
        case DisplayRequestType::DRAW_MENU:
            display->showMenu(
                request.data.menu.title,
                request.data.menu.items,
                request.data.menu.count,
                request.data.menu.selected,
                request.data.menu.hwState
            );
            break;

        case DisplayRequestType::SHOW_STATUS:
            display->showStatus(
                request.data.status.key,
                request.data.status.value
            );
            break;

        case DisplayRequestType::SHOW_MESSAGE:
            display->showMessage(request.data.message.value);
            break;

        case DisplayRequestType::CLEAR:
            display->clear();
            break;

        case DisplayRequestType::CLEAR_WARNING:
            // Clear warning and restore last normal mode state
            display->clear();
            display->drawNormalMode(lastNormalModeState);
            LOG_DEBUG(TAG, "Warning cleared, normal mode restored");
            break;

        case DisplayRequestType::DRAW_NORMAL_MODE:
            // Cache state for restoration after warning
            lastNormalModeState = request.data.normalMode.hwState;
            display->drawNormalMode(request.data.normalMode.hwState);
            break;
    }
}
