#ifndef INA780_H
#define INA780_H

void ina780_init(void);
float ina780_read_current(void);
float ina780_read_bus_voltage(void);
float ina780_read_temperature(void);

#endif // INA780_H

