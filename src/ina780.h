#ifndef INA780_H
#define INA780_H

void ina780_init(void);
float ina780_read_current(void);      // Reads current in amperes (A)
float ina780_read_bus_voltage(void);  // Reads Votage in volts (V)
float ina780_read_temperature(void);  // Read the internal die temperature in degrees Celsius
// TODO: Add check in either main or here to alarm if overheating
float ina780_read_power(void);    // Return power consumption in watts (W)
double ina780_read_energy(void);  // Return engergy consumtion from battry packs in watt-hours (Wh)

#endif  // INA780_H
