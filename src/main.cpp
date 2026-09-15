#include <Arduino.h>

#include "Buttons.h"
#include "LoadCell.h"
#include "DataLogger.h"
#include "StateMachine.h"

Buttons buttons;
LoadCell loadCell;
DataLogger logger;

StateMachine stateMachine(
    buttons,
    loadCell,
    logger
);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    buttons.begin();

    if (!loadCell.begin())
    {
        Serial.println("ERROR: HX711 not ready!");

        while (true)
        {
            delay(1000);
        }
    }

    if (!logger.begin())
    {
        Serial.println("ERROR: SD card initialization failed!");

        while (true)
        {
            delay(1000);
        }
    }

    stateMachine.begin();
}

void loop()
{
    stateMachine.update();
}