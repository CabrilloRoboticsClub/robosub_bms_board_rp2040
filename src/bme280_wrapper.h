#ifndef BME280_WRAPPER_H
#define BME280_WRAPPER_H

#include <stdbool.h>

bool bme280_wrapper_init(void);
bool bme280_read_all(float *temp, float *hum, float *press);

#endif
