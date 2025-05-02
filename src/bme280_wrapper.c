#define BME280_INTERFACE_I2C
#define BME280_FLOAT_ENABLE

#include "bme280.h"
#include "bme280_wrapper.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define BME280_I2C       i2c1
#define BME280_SDA_PIN   2
#define BME280_SCL_PIN   3
#define BME280_ADDR      0x77

#define BME280_SLEEP_MODE   0x00
#define BME280_FORCED_MODE  0x01
#define BME280_NORMAL_MODE  0x03

#define BME280_PRESS        0x01
#define BME280_TEMP         0x02
#define BME280_HUM          0x04
#define BME280_ALL          0x07

static struct bme280_dev dev;
static uint8_t dev_addr = BME280_ADDR;

static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
    uint8_t addr = *(uint8_t *)intf_ptr;
    int ret = i2c_write_blocking(BME280_I2C, addr, &reg_addr, 1, true);
    if (ret < 0) return -1;
    ret = i2c_read_blocking(BME280_I2C, addr, data, len, false);
    return (ret < 0) ? -1 : 0;
}

static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
    uint8_t addr = *(uint8_t *)intf_ptr;
    uint8_t buf[len + 1];
    buf[0] = reg_addr;
    for (uint32_t i = 0; i < len; ++i) buf[i + 1] = data[i];
    int ret = i2c_write_blocking(BME280_I2C, addr, buf, len + 1, false);
    return (ret < 0) ? -1 : 0;
}

static void user_delay_ms(uint32_t period) {
    sleep_ms(period);
}

bool bme280_wrapper_init(void) {
    i2c_init(BME280_I2C, 100 * 1000);
    gpio_set_function(BME280_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BME280_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(BME280_SDA_PIN);
    gpio_pull_up(BME280_SCL_PIN);

    dev.intf_ptr = &dev_addr;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_ms;

    if (bme280_init(&dev) != BME280_OK) return false;

    struct bme280_settings settings = {
        .osr_h = BME280_OVERSAMPLING_1X,
        .osr_p = BME280_OVERSAMPLING_1X,
        .osr_t = BME280_OVERSAMPLING_1X,
        .filter = BME280_FILTER_COEFF_OFF
    };

    if (bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &dev) != BME280_OK) return false;

    return true;
}

bool bme280_read_all(float *temp, float *hum, float *press) {
    int8_t status;

    status = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    if (status != BME280_OK) {
        printf("Error: bme280_set_sensor_mode failed: %d\n", status);
        return false;
    }

    user_delay_ms(40);

    struct bme280_data data;
    status = bme280_get_sensor_data(BME280_ALL, &data, &dev);
    if (status != BME280_OK) {
        printf("Error: bme280_get_sensor_data failed: %d\n", status);
        return false;
    }

    *temp = data.temperature;
    *hum  = data.humidity;
    *press = data.pressure / 100.0;

    return true;
}
