#include "Buttons.h"
#include "Config.h"


void Buttons::begin()
{
    tarePin = Config::TARE_BUTTON_PIN;
    startPin = Config::START_BUTTON_PIN;

    pinMode(tarePin, INPUT_PULLUP);
    pinMode(startPin, INPUT_PULLUP);

    tareStableState = HIGH;
    startStableState = HIGH;

    tareLastReading = HIGH;
    startLastReading = HIGH;

    tareLastChangeTime = millis();
    startLastChangeTime = millis();

    tareEvent = false;
    startEvent = false;
}


void Buttons::update()
{
    uint32_t now = millis();

    bool tareReading = digitalRead(tarePin);
    bool startReading = digitalRead(startPin);


    // ============================================================
    // TARE button
    // ============================================================

    if (tareReading != tareLastReading)
    {
        tareLastChangeTime = now;
        tareLastReading = tareReading;
    }

    if ((now - tareLastChangeTime) >= DEBOUNCE_MS)
    {
        if (tareReading != tareStableState)
        {
            tareStableState = tareReading;

            // Button is active LOW.
            if (tareStableState == LOW)
            {
                tareEvent = true;
            }
        }
    }


    // ============================================================
    // START button
    // ============================================================

    if (startReading != startLastReading)
    {
        startLastChangeTime = now;
        startLastReading = startReading;
    }

    if ((now - startLastChangeTime) >= DEBOUNCE_MS)
    {
        if (startReading != startStableState)
        {
            startStableState = startReading;

            // Button is active LOW.
            if (startStableState == LOW)
            {
                startEvent = true;
            }
        }
    }
}


bool Buttons::tarePressed()
{
    if (tareEvent)
    {
        tareEvent = false;
        return true;
    }

    return false;
}


bool Buttons::startPressed()
{
    if (startEvent)
    {
        startEvent = false;
        return true;
    }

    return false;
}