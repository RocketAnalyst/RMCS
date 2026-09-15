#include "DataLogger.h"
#include "Config.h"

bool DataLogger::begin()
{
    if (!SD.begin(BUILTIN_SDCARD))
    {
        return false;
    }

    return true;
}

bool DataLogger::startTest(const LoadCell &loadCell)
{
    if (logging)
    {
        return false;
    }

    testNumber = findNextTestNumber();

    if (!buildFilename(
            testNumber,
            filename,
            sizeof(filename)))
    {
        return false;
    }

    logFile = SD.open(filename, FILE_WRITE);

    if (!logFile)
    {
        return false;
    }

    // Reset sample numbering for every new test.
    sampleNumber = 0;

    logFile.print("RMCS Version,");
    logFile.println("0.01");

    logFile.print("Test Number,");
    logFile.println(testNumber);

    logFile.print("Calibration Counts Per Newton,");
    logFile.println(loadCell.getCountsPerNewton(), 6);

    logFile.print("Tare Raw,");
    logFile.println(loadCell.getTareRaw());

    logFile.println();

    logFile.println(
        "sample,time_ms,raw_hx711,delta,thrust_N,thrust_lbf,state"
    );

    logFile.flush();

    lastFlushTime = millis();
    logging = true;

    return true;
}

bool DataLogger::writeMeasurement(
    const Measurement &measurement,
    const char *state)
{
    if (!logging)
    {
        return false;
    }

    logFile.print(sampleNumber++);
    logFile.print(",");
    logFile.print(measurement.timeMs);
    logFile.print(",");
    logFile.print(measurement.raw);
    logFile.print(",");
    logFile.print(measurement.delta);
    logFile.print(",");
    logFile.print(measurement.forceN, 6);
    logFile.print(",");
    logFile.print(measurement.forceLbf, 6);
    logFile.print(",");
    logFile.println(state);

    return true;
}

void DataLogger::update()
{
    if (!logging)
    {
        return;
    }

    uint32_t now = millis();

    if ((now - lastFlushTime) >= Config::SD_FLUSH_INTERVAL_MS)
    {
        logFile.flush();
        lastFlushTime = now;
    }
}

void DataLogger::finish()
{
    if (!logging)
    {
        return;
    }

    logFile.flush();
    logFile.close();

    logging = false;
}

bool DataLogger::isLogging() const
{
    return logging;
}

uint32_t DataLogger::getTestNumber() const
{
    return testNumber;
}

const char *DataLogger::getFilename() const
{
    return filename;
}

uint32_t DataLogger::findNextTestNumber()
{
    uint32_t highestNumber = 0;

    File root = SD.open("/");

    if (!root)
    {
        return 1;
    }

    File entry;

    while (true)
    {
        entry = root.openNextFile();

        if (!entry)
        {
            break;
        }

        if (!entry.isDirectory())
        {
            const char *name = entry.name();

            if (strncmp(
                    name,
                    Config::TEST_FILE_PREFIX,
                    strlen(Config::TEST_FILE_PREFIX)
                ) == 0)
            {
                unsigned long number = 0;

                char format[32];

                snprintf(
                    format,
                    sizeof(format),
                    "%s%%lu%s",
                    Config::TEST_FILE_PREFIX,
                    Config::TEST_FILE_EXTENSION
                );

                if (sscanf(name, format, &number) == 1)
                {
                    if (number > highestNumber)
                    {
                        highestNumber = number;
                    }
                }
            }
        }

        entry.close();
    }

    root.close();

    return highestNumber + 1;
}

bool DataLogger::buildFilename(
    uint32_t number,
    char *buffer,
    size_t bufferSize)
{
    int result = snprintf(
        buffer,
        bufferSize,
        "%s%03lu%s",
        Config::TEST_FILE_PREFIX,
        static_cast<unsigned long>(number),
        Config::TEST_FILE_EXTENSION
    );

    return result > 0 &&
           static_cast<size_t>(result) < bufferSize;
}