#include "StateMachine.h"
#include "Config.h"


StateMachine::StateMachine(
    Buttons &buttons,
    LoadCell &loadCell,
    DataLogger &logger
)
    : buttons(buttons),
      loadCell(loadCell),
      logger(logger)
{
}


void StateMachine::begin()
{
    state = SystemState::READY;

    hasTare = false;

    testStartTime = 0;

    ignitionStartTime = 0;
    ignitionThresholdActive = false;

    burnoutStartTime = 0;
    burnoutThresholdActive = false;

    postBurnStartTime = 0;


    Serial.println();
    Serial.println("==============================");
    Serial.println("RMCS State Machine");
    Serial.println("==============================");
    Serial.println("State: READY");
    Serial.println("Perform TARE before START.");
}


void StateMachine::update()
{
    // Update button states first.
    buttons.update();


    // Handle the current state.
    switch (state)
    {
        case SystemState::READY:
            handleReady();
            break;


        case SystemState::WAITING_FOR_IGNITION:
        case SystemState::BURNING:
        case SystemState::POST_BURN:
            handleRecording();
            break;
    }


    // Allow the logger to perform periodic SD flushing.
    logger.update();
}


void StateMachine::handleReady()
{
    // ============================================================
    // TARE
    // ============================================================

    if (buttons.tarePressed())
    {
        Serial.println("TARE requested...");


        if (loadCell.tare())
        {
            hasTare = true;

            Serial.print("TARE complete. Raw = ");
            Serial.println(loadCell.getTareRaw());
        }
        else
        {
            Serial.println("ERROR: TARE failed!");
        }
    }


    // ============================================================
    // START
    // ============================================================

    if (buttons.startPressed())
    {
        // START requires a valid tare.
        if (!hasTare)
        {
            Serial.println(
                "START ignored: TARE required first."
            );

            return;
        }


        // Create the next test file.
        if (!logger.startTest(loadCell))
        {
            Serial.println(
                "ERROR: Could not start data logger!"
            );

            return;
        }


        // START means recording begins immediately.
        testStartTime = millis();


        // Reset ignition detector.
        ignitionStartTime = 0;
        ignitionThresholdActive = false;


        // Reset burnout detector.
        burnoutStartTime = 0;
        burnoutThresholdActive = false;


        // Reset post-burn timer.
        postBurnStartTime = 0;


        Serial.println();
        Serial.println("==============================");
        Serial.print("TEST ");
        Serial.print(logger.getTestNumber());
        Serial.println(" STARTED");

        Serial.print("File: ");
        Serial.println(logger.getFilename());

        Serial.println("==============================");


        changeState(
            SystemState::WAITING_FOR_IGNITION
        );
    }
}


void StateMachine::handleRecording()
{
    uint32_t now = millis();


    Measurement measurement;


    // Get one actual load-cell measurement.
    if (!loadCell.read(
            measurement,
            now - testStartTime))
    {
        return;
    }


    // ============================================================
    // WAITING FOR IGNITION
    // ============================================================

    if (state == SystemState::WAITING_FOR_IGNITION)
    {
        handleWaitingForIgnition(measurement);
    }


    // ============================================================
    // BURNING
    // ============================================================

    else if (state == SystemState::BURNING)
    {
        handleBurning(measurement);
    }


    // ============================================================
    // POST BURN
    // ============================================================

    else if (state == SystemState::POST_BURN)
    {
        handlePostBurn(measurement);
    }


    // ============================================================
    // Write every measurement
    // ============================================================

    logger.writeMeasurement(
        measurement,
        stateName(state)
    );


    // ============================================================
    // Finish after 5 seconds of post-burn recording
    // ============================================================

    if (state == SystemState::POST_BURN)
    {
        if ((now - postBurnStartTime) >=
            Config::POST_BURN_RECORD_MS)
        {
            logger.finish();


            Serial.println();
            Serial.println("==============================");
            Serial.println("TEST COMPLETE");

            Serial.print("File: ");
            Serial.println(logger.getFilename());

            Serial.println("==============================");


            changeState(
                SystemState::READY
            );
        }
    }
}


void StateMachine::handleWaitingForIgnition(
    const Measurement &measurement
)
{
    // Use absolute force because the mechanical arrangement
    // determines the sign of the load-cell signal.
    double force = abs(measurement.forceN);


    // ------------------------------------------------------------
    // Above ignition threshold
    // ------------------------------------------------------------

    if (force >= Config::IGNITION_THRESHOLD_N)
    {
        // Start the confirmation timer.
        if (!ignitionThresholdActive)
        {
            ignitionThresholdActive = true;
            ignitionStartTime = millis();
        }


        // Require the signal to remain above the threshold
        // for the configured confirmation period.
        if ((millis() - ignitionStartTime) >=
            Config::IGNITION_CONFIRM_MS)
        {
            Serial.println();
            Serial.println("*** IGNITION DETECTED ***");


            ignitionThresholdActive = false;


            changeState(
                SystemState::BURNING
            );
        }
    }


    // ------------------------------------------------------------
    // Signal dropped back below threshold
    // ------------------------------------------------------------

    else
    {
        ignitionThresholdActive = false;
    }
}


void StateMachine::handleBurning(
    const Measurement &measurement
)
{
    // Use absolute force for detection.
    double force = abs(measurement.forceN);


    // ------------------------------------------------------------
    // Below burnout threshold
    // ------------------------------------------------------------

    if (force <= Config::BURNOUT_THRESHOLD_N)
    {
        // Start the confirmation timer.
        if (!burnoutThresholdActive)
        {
            burnoutThresholdActive = true;
            burnoutStartTime = millis();
        }


        // Require the signal to remain below the threshold
        // for the configured confirmation period.
        if ((millis() - burnoutStartTime) >=
            Config::BURNOUT_CONFIRM_MS)
        {
            Serial.println();
            Serial.println("*** BURNOUT DETECTED ***");


            burnoutThresholdActive = false;


            // Begin the 5-second post-burn recording period.
            postBurnStartTime = millis();


            changeState(
                SystemState::POST_BURN
            );
        }
    }


    // ------------------------------------------------------------
    // Signal rose above threshold again
    // ------------------------------------------------------------

    else
    {
        burnoutThresholdActive = false;
    }
}


void StateMachine::handlePostBurn(
    const Measurement &measurement
)
{
    // Nothing needs to happen here.
    //
    // Measurements continue to be recorded.
    // handleRecording() will stop the test after
    // POST_BURN_RECORD_MS has elapsed.
}


void StateMachine::changeState(
    SystemState newState
)
{
    if (state == newState)
    {
        return;
    }


    state = newState;


    Serial.print("State -> ");
    Serial.println(stateName(state));
}


SystemState StateMachine::getState() const
{
    return state;
}


const char *StateMachine::stateName(
    SystemState state
) const
{
    switch (state)
    {
        case SystemState::READY:
            return "READY";


        case SystemState::WAITING_FOR_IGNITION:
            return "WAITING_FOR_IGNITION";


        case SystemState::BURNING:
            return "BURNING";


        case SystemState::POST_BURN:
            return "POST_BURN";


        default:
            return "UNKNOWN";
    }
}