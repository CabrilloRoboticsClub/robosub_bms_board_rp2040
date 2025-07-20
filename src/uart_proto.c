#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "uart_proto.h"
#include "crc.h"
#include "bme280_wrapper.h"
#include "ina780.h"
#include "gpio_control.h"

#define BMS 1

uint32_t uart_read_blocking_timeout(uart_inst_t *uart, uint8_t *dst, uint32_t len) {
    uint32_t bytes_read = 0;

    for (uint32_t i = 0; i < len; ++i) {
        if (uart_is_readable_within_us(uart, TIMEOUT)) {
            dst[i] = (uint8_t) uart_getc(uart);
            bytes_read++;
        } else {
            break;
        }
    }

    return bytes_read;
}

bool uart_initialization(uart_inst inst) {
    gpio_set_function(inst.tx, GPIO_FUNC_UART);
    gpio_set_function(inst.rx, GPIO_FUNC_UART);
    uart_init(inst.uart, inst.baudrate);
    return true;
}

bool get_response(uart_inst inst, sensor_data* retval) {
	// Attempt memory allocation
	uint8_t* resp = (uint8_t *) malloc(HEADER_SIZE);
	if (resp == NULL) {
		printf("Memory allocation failed\n");
		return false;
	} 

	// Read into allocated block
	uint32_t num_bytes = uart_read_blocking_timeout(inst.uart, resp, HEADER_SIZE);
	if (num_bytes != HEADER_SIZE) {
		printf("Failed to read header, expected %d bytes, got %d\n", HEADER_SIZE, num_bytes);
		free(resp);
		return false;
	}

	msg_type type = resp[0];
	uint8_t byte_count = resp[1];

	printf("Received message type: %d, byte count: %d\n", type, byte_count);

	// Attempt to reallocate
	uint8_t* new_resp = (uint8_t *) realloc(resp, HEADER_CRC_SIZE + byte_count);
	if (new_resp == NULL) {
		free(resp);  // avoid leak if realloc fails
		return false;
	}
	resp = new_resp;

	// Read into reallocated block
	num_bytes = uart_read_blocking_timeout(inst.uart, resp + PAYLOAD_OFFSET, CRC_SIZE + byte_count);
	if (num_bytes != CRC_SIZE + byte_count) {
		printf("Failed to read payload, expected %d bytes, got %d\n", CRC_SIZE + byte_count, num_bytes);
		free(resp);
		return false;
	}

	printf("Received payload of size: %d\n", num_bytes);

	bool success = true;

	switch(type) {
		case data:
			if (byte_count != DATA_PAYLOAD_SIZE || !check_crc(resp, DATA_SIZE_NO_CRC)) {
				success = false;
			} else {
				*retval = parse_data(resp);
				break;
			}
		case request:
#if BMS == 1
			if (byte_count != REQUEST_PAYLOAD_SIZE || !check_crc(resp, REQUEST_SIZE_NO_CRC)) {
				printf("Invalid request response: byte count %d, expected %d\n", byte_count, REQUEST_PAYLOAD_SIZE);
				success = false;
			} else {
				printf("Sending data in response to request\n");
				send_data(inst);
			}
			break;
#else
			success = false;
			break;
#endif
		default:
			success = false;
			break;
	}
	free(resp);
	return success;

}

bool send_request(uart_inst inst) {
	uint8_t* msg = (uint8_t *) malloc(REQUEST_SIZE_NO_CRC);
	msg[0] = request;
	msg[1] = REQUEST_PAYLOAD_SIZE;

	add_crc_byte(msg, REQUEST_SIZE_NO_CRC);
	uart_write_blocking(inst.uart, msg, REQUEST_SIZE);

	free(msg);
	return true;
}

#if BMS == 1
bool send_data(uart_inst inst) {
	// Make readings
	float bme280_temperature;
	float bme280_hum;
	float bme280_press;

	bme280_read_all(&bme280_temperature, &bme280_hum, &bme280_press);

	printf("INA780 readings:\n");
	printf("Current: %.2f A\n", ina780_read_current());
	printf("Voltage: %.2f V\n", ina780_read_bus_voltage());
	printf("Temperature: %.2f °C\n", ina780_read_temperature());
	printf("Power: %.2f W\n", ina780_read_power());
	printf("Energy: %.2f Wh\n", ina780_read_energy());
	sensor_data sen_data = {
		gpio_kill_switch_triggered(),
		ina780_read_current(),
		ina780_read_bus_voltage(),
		ina780_read_temperature(),
		ina780_read_power(),
		ina780_read_energy(),
		bme280_temperature,
		bme280_hum,
		bme280_press,
	};

	// Initialize msg
	uint8_t bytes = DATA_PAYLOAD_SIZE;
	uint8_t* msg = (uint8_t *) malloc(DATA_SIZE_NO_CRC); // Header + byte count + payload

	if (msg == NULL) return 0;

	// Fill msg with data
	msg[HEADER_OFFSET] = data;
	msg[BYTE_COUNT_OFFSET] = bytes;

	memcpy(msg + PAYLOAD_OFFSET, &sen_data, bytes);

	// Add crc and transmit
	add_crc_byte(msg, DATA_SIZE_NO_CRC);
	uart_write_blocking(inst.uart, msg, DATA_SIZE); // Header + byte count + payload + crc

	free(msg);

	return true;
}
#endif

sensor_data parse_data(uint8_t* resp) {
	sensor_data retval;
	memcpy(&retval, resp + PAYLOAD_OFFSET, DATA_PAYLOAD_SIZE);
	return retval;
}