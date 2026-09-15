#pragma once

#include <Arduino.h>


class Buttons
{
public:

    // Initialize both buttons.
    void begin();

    // Call this continuously from the main loop.
    void update();

    // Returns true once when TARE is pressed.
    bool tarePressed();

    // Returns true once when START is pressed.
    bool startPressed();


private:

    static constexpr uint32_t DEBOUNCE_MS = 50;

    uint8_t tarePin;
    uint8_t startPin;

    bool tareStableState;
    bool startStableState;

    bool tareLastReading;
    bool startLastReading;

    uint32_t tareLastChangeTime;
    uint32_t startLastChangeTime;

    bool tareEvent;
    bool startEvent;
};