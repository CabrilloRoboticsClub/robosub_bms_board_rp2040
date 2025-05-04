#include "ina780.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define INA780_I2C         i2c1
#define INA780_ADDR        0x40 //40
#define INA780_SDA_PIN     2
#define INA780_SCL_PIN     3

#define INA780_REG_CONFIG          0x00
#define INA780_REG_ADC_CONFIG      0x01
#define INA780_REG_VBUS            0x05
#define INA780_REG_DIETEMP         0x06
#define INA780_REG_CURRENT         0x07
#define INA780_REG_POWER           0x08
#define INA780_REG_ENERGY          0x09
#define INA780_REG_CHARGE          0x0A

#define MSB(x) (((x) >> 8) & 0xFF)
#define LSB(x) ((x) & 0xFF)

static uint8_t ina780_address = INA780_ADDR;

static int i2c_write16(uint8_t reg, uint16_t val) {
    uint8_t buf[3] = {reg, MSB(val), LSB(val)};
    return i2c_write_blocking(INA780_I2C, ina780_address, buf, 3, false);
}

static int i2c_read(uint8_t reg, uint8_t *data, size_t len) {
    int res = i2c_write_blocking(INA780_I2C, ina780_address, &reg, 1, true);
    if (res < 0) return res;
    return i2c_read_blocking(INA780_I2C, ina780_address, data, len, false);
}

static int16_t to_int16(uint8_t *data) {
    return (int16_t)((data[0] << 8) | data[1]);
}

void ina780_init() {
    i2c_init(INA780_I2C, 100 * 1000);
    gpio_set_function(INA780_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(INA780_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(INA780_SDA_PIN);
    gpio_pull_up(INA780_SCL_PIN);

    // Write default config
    i2c_write16(INA780_REG_CONFIG, 0x0000);
    i2c_write16(INA780_REG_ADC_CONFIG, 0xFB68); // Continuous V/I/T mode, default conv time
}

float ina780_read_current() {
    uint8_t buf[2];
    if (i2c_read(INA780_REG_CURRENT, buf, 2) != 2) return -1.0f;
    int16_t raw = to_int16(buf);
    return raw * 2.4f / 1000.0f; // mA to A
}

float ina780_read_bus_voltage() {
    uint8_t buf[2];
    if (i2c_read(INA780_REG_VBUS, buf, 2) != 2) return -1.0f;
    uint16_t raw = (buf[0] << 8) | buf[1];
    return raw * 3.125f / 1000.0f;
}

float ina780_read_temperature() {
    uint8_t buf[2];
    if (i2c_read(INA780_REG_DIETEMP, buf, 2) != 2) return -1000.0f;
    int16_t raw = ((buf[0] << 8) | buf[1]) >> 4;
    if (raw & 0x0800) raw |= 0xF000; // sign-extend
    return raw * 0.125f;
}
