#pragma once

#include <Arduino.h>

namespace Config
{
    // ============================================================
    // Hardware Pins
    // ============================================================

    // SparkFun HX711
    constexpr uint8_t HX711_SCK_PIN = 2;
    constexpr uint8_t HX711_DAT_PIN = 3;

    // Buttons
    constexpr uint8_t TARE_BUTTON_PIN  = 4;
    constexpr uint8_t START_BUTTON_PIN = 5;


    // ============================================================
    // Load Cell / Calibration
    // ============================================================

    // Preliminary calibration value.
    // This WILL be refined later with a better calibration fixture.
    constexpr double COUNTS_PER_NEWTON = 226.0;

    // Number of HX711 samples used when performing a tare.
    constexpr uint8_t TARE_SAMPLES = 200;


    // ============================================================
    // Flight / Test Detection
    // ============================================================

    // Minimum force required before we consider ignition detected.
    constexpr double IGNITION_THRESHOLD_N = 5.0;

    // Ignition must remain above the threshold for this long.
    constexpr uint32_t IGNITION_CONFIRM_MS = 50;

    // Force must remain below this threshold for this long
    // before burnout is considered detected.
    constexpr double BURNOUT_THRESHOLD_N = 5.0;

    constexpr uint32_t BURNOUT_CONFIRM_MS = 500;


    // ============================================================
    // Post-Burn Recording
    // ============================================================

    // Continue recording for 5 seconds after burnout.
    constexpr uint32_t POST_BURN_RECORD_MS = 5000;


    // ============================================================
    // Data Logger
    // ============================================================

    // Flush the SD file periodically rather than after every sample.
    constexpr uint32_t SD_FLUSH_INTERVAL_MS = 1000;


    // ============================================================
    // File Naming
    // ============================================================

    // Test files will be:
    //
    // TEST_001.CSV
    // TEST_002.CSV
    // TEST_003.CSV
    //
    // Existing files are never overwritten.
    constexpr const char *TEST_FILE_PREFIX = "TEST_";
    constexpr const char *TEST_FILE_EXTENSION = ".CSV";
}