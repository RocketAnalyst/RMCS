#pragma once

#include <Arduino.h>

#include "Buttons.h"
#include "LoadCell.h"
#include "DataLogger.h"


enum class SystemState
{
    READY,
    WAITING_FOR_IGNITION,
    BURNING,
    POST_BURN
};


class StateMachine
{
public:

    StateMachine(
        Buttons &buttons,
        LoadCell &loadCell,
        DataLogger &logger
    );

    // Initialize the state machine.
    void begin();

    // Run one iteration of the state machine.
    void update();

    // Get the current state.
    SystemState getState() const;


private:

    // State handlers.
    void handleReady();

    void handleRecording();

    void handleWaitingForIgnition(
        const Measurement &measurement
    );

    void handleBurning(
        const Measurement &measurement
    );

    void handlePostBurn(
        const Measurement &measurement
    );

    // Change the current state.
    void changeState(SystemState newState);

    // Convert a state to a printable name.
    const char *stateName(SystemState state) const;


    // Hardware/software modules.
    Buttons &buttons;
    LoadCell &loadCell;
    DataLogger &logger;


    // Current state.
    SystemState state = SystemState::READY;


    // Tare must be performed before START.
    bool hasTare = false;


    // Time START was pressed.
    uint32_t testStartTime = 0;


    // Ignition detection.
    uint32_t ignitionStartTime = 0;
    bool ignitionThresholdActive = false;


    // Burnout detection.
    uint32_t burnoutStartTime = 0;
    bool burnoutThresholdActive = false;


    // Time burnout was detected.
    uint32_t postBurnStartTime = 0;
};