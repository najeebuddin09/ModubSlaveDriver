/*
 * modbus.h
 *
 *  Created on: Sep 23, 2020
 *      Author: Najeeb Uddin
 */

#ifndef CUSTOM_LIBS_MODBUS_H_
#define CUSTOM_LIBS_MODBUS_H_

#include "sci.h"
#include "DSP28x_Project.h"
#include "custom_headers/firmware_version.h"
#include "custom_libs/eeprom.h"

#define SLAVE_ID 0x01
#define FUNCTION_READ_COILS 0x01
#define FUNCTION_READ_DISCRETE_INPUTS 0x02
#define FUNCTION_READ_HOLDING_REGISTERS 0x03
#define FUNCTION_READ_INPUT_REGISTER 0x04
#define FUNCTION_WRITE_SINGLE_COIL 0x05
#define FUNCTION_WRITE_SINGLE_REGISTER 0x06
#define FUNCTION_READ_EXCEPTION_STATUS 0x07

#define EXCEPTION_CODE_1 0x01
#define EXCEPTION_CODE_2 0x02
#define EXCEPTION_CODE_3 0x03

#define RESPONSE_FRAME_BUFFER 75

#define NUMBER_OF_HOLDING_REGISTERS RESPONSE_FRAME_BUFFER
#define NUMBER_OF_COILS_REGISTERS RESPONSE_FRAME_BUFFER
#define NUMBER_OF_DISCRETE_INPUT RESPONSE_FRAME_BUFFER

void modbus_response_read_registers(Uint8 FUNC, Uint16 number_of_registers, Uint16 starting_address, Uint16 * array);
//void modbus_response_read_bits(Uint8 FUNC, Uint16 number_of_coils, Uint16 starting_coil_address, Uint16 * array);
void modbus_response_write_register(Uint8 * array);
Uint16 ModRTU_CRC(Uint8 * , int );
Uint8 get_bit(Uint8 byte, Uint8 position);
void modbus_error_response(Uint8 function_code, Uint8 exception_code);
void modbus_holding_register_init(Uint16 * holding_registers, Uint8 length);

#endif /* CUSTOM_LIBS_MODBUS_H_ */
