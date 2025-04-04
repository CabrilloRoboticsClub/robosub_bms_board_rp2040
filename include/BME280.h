#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

namespace robosub {

class BME280Sensor {
public:
    void init();
    float readTemperature();
    float readHumidity();
    float readPressure();
};

} // namespace robosub

#endif
