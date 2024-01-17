#include "CarInfoStubImpl.hpp"


CarInfoStubImpl::CarInfoStubImpl() {
}

CarInfoStubImpl::~CarInfoStubImpl() {
}

/**
 * @brief Sets the battery information in the car.
 * 
 * This function sets the battery level, voltage, current, and consumption
 * in the car's battery struct.
 * 
 * @param level_ The battery level.
 * @param voltage_ The battery voltage.
 * @param current_ The battery current.
 * @param consumption_ The battery consumption.
 */
void CarInfoStubImpl::setBatteryStruct(float level_, float voltage_, float current_, float consumption_) {
    v0::commonapi::CarInfo::batteryStruct battery;
    battery.setLevel(level_);
    battery.setVoltage(voltage_);
    battery.setCurrent(current_);
    battery.setConsumption(consumption_);
    setBatteryAttribute(battery);
}