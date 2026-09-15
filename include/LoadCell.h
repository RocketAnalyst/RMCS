#pragma once

#include <Arduino.h>

#include <HX711.h>

// A single measurement from the load cell.
struct Measurement
{
    uint32_t timeMs;
    long raw;
    long delta;
    double forceN;
    double forceLbf;
};


class LoadCell
{
public:

    // Initialize the HX711.
    bool begin();

    // Check whether the HX711 is responding.
    bool isReady();

    // Perform a tare operation.
    bool tare();

    // Read one measurement.
    bool read(Measurement &measurement, uint32_t timeMs);

    // Get the current tare value.
    long getTareRaw() const;

    // Get the calibration factor.
    double getCountsPerNewton() const;


private:

    HX711 scale;

    long tareRaw = 0;
};