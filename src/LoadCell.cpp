#include "LoadCell.h"
#include "Config.h"


bool LoadCell::begin()
{
    scale.begin(
        Config::HX711_DAT_PIN,
        Config::HX711_SCK_PIN
    );

    return isReady();
}


bool LoadCell::isReady()
{
    return scale.is_ready();
}


bool LoadCell::tare()
{
    if (!isReady())
    {
        return false;
    }

    tareRaw = scale.read_average(Config::TARE_SAMPLES);

    return true;
}


bool LoadCell::read(Measurement &measurement, uint32_t timeMs)
{
    if (!isReady())
    {
        return false;
    }

    long raw = scale.read();

    long delta = raw - tareRaw;

    double forceN =
        static_cast<double>(delta) /
        Config::COUNTS_PER_NEWTON;

    double forceLbf =
        forceN * 0.224808943;

    measurement.timeMs = timeMs;
    measurement.raw = raw;
    measurement.delta = delta;
    measurement.forceN = forceN;
    measurement.forceLbf = forceLbf;

    return true;
}


long LoadCell::getTareRaw() const
{
    return tareRaw;
}


double LoadCell::getCountsPerNewton() const
{
    return Config::COUNTS_PER_NEWTON;
}