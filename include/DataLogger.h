#pragma once

#include <Arduino.h>
#include <SD.h>

#include "LoadCell.h"

class DataLogger
{
public:
    bool begin();
    bool startTest(const LoadCell &loadCell);
    bool writeMeasurement(const Measurement &measurement, const char *state);
    void update();
    void finish();

    bool isLogging() const;
    uint32_t getTestNumber() const;
    const char *getFilename() const;

private:
    uint32_t findNextTestNumber();
    bool buildFilename(
        uint32_t testNumber,
        char *buffer,
        size_t bufferSize
    );

    File logFile;

    bool logging = false;

    uint32_t testNumber = 0;
    uint32_t sampleNumber = 0;

    char filename[32] = {0};

    uint32_t lastFlushTime = 0;
};