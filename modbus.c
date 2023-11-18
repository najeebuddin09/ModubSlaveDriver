/*
 * modbus.c
 *
 *  Created on: Sep 23, 2020
 *      Author: Najeeb Uddin
 */

#include "modbus.h"

Uint8 modbus_response_frame[6 + RESPONSE_FRAME_BUFFER * 2] = {};

void modbus_response_read_registers(Uint8 FUNC, Uint16 number_of_registers, Uint16 starting_address, Uint16 * array){
  Uint8 i = starting_address, j=0, k=0;
  Uint16 crc = 0; 
  modbus_response_frame[j++] = SLAVE_ID;
  modbus_response_frame[j++] = FUNC;
  modbus_response_frame[j++] = (number_of_registers & 0xff)*2;
  while (i < starting_address + number_of_registers){
    modbus_response_frame[j++] = array[i]>>8;
    modbus_response_frame[j++] = array[i++] & 0xff;
  }
  crc = ModRTU_CRC(modbus_response_frame,j);
  modbus_response_frame[j++] = crc & 0xff;
  modbus_response_frame[j++] = crc >> 8;
  for (k=0; k<j; k++){
    scia_transmit_byte(modbus_response_frame[k],CHAR);
  }
  //modbus_response_frame[j++] = '\0';
  //scia_transmit_string((const char *)modbus_response_frame);
}

/*
void modbus_response_read_bits(Uint8 FUNC, Uint16 number_of_coils, Uint16 starting_coil_address, Uint16 * array){
  Uint8 i=starting_coil_address, j=0, s=0;
  Uint16 crc = 0;
  Uint8 number_of_registers = (((number_of_coils & 0xff)-1)/8)+1;
  Uint8 flush_index = 0;
  for (flush_index = 0; flush_index < 6 + number_of_registers; flush_index++){
    modbus_response_frame[flush_index] = 0;
  }
  modbus_response_frame[j++] = SLAVE_ID;
  modbus_response_frame[j++] = FUNC;
  modbus_response_frame[j++] = number_of_registers;
  while (i < starting_coil_address + number_of_coils){
    modbus_response_frame[j+(s/8)] |= get_bit(array[i/8],i%8)<<(s%8);
    i++;
    s++;
  }
  j+=number_of_registers;
  crc = ModRTU_CRC(modbus_response_frame,j);
  modbus_response_frame[j++] = crc & 0xff;
  modbus_response_frame[j++] = crc >> 8;
  modbus_response_frame[j++] = '\0';
  scia_transmit_string((const char *)modbus_response_frame);
}
*/

void modbus_response_write_register(Uint8 * array){
  scia_transmit_byte(array[0], CHAR);
  scia_transmit_byte(array[1], CHAR);
  scia_transmit_byte(array[2], CHAR);
  scia_transmit_byte(array[3], CHAR);
  scia_transmit_byte(array[4], CHAR);
  scia_transmit_byte(array[5], CHAR);
  scia_transmit_byte(array[6], CHAR);
  scia_transmit_byte(array[7], CHAR);
}

void modbus_error_response(Uint8 function_code, Uint8 exception_code)
{
  Uint16 crc = 0;
  Uint8 j = 0;
  modbus_response_frame[j++] = SLAVE_ID;
  modbus_response_frame[j++] = function_code | 0x80;
  modbus_response_frame[j++] = exception_code;
  crc = ModRTU_CRC(modbus_response_frame,j);
  modbus_response_frame[j++] = crc & 0xff;
  modbus_response_frame[j++] = crc >> 8;
  modbus_response_frame[j++] = '\0';
  scia_transmit_string((const char *)modbus_response_frame);
}


Uint8 get_bit(Uint8 byte, Uint8 position)
{
  return (byte>>(position)) & 0x01; 
}

Uint16 ModRTU_CRC(Uint8 * buf, int len)
{
  Uint16 crc = 0xFFFF;
  int pos = 0;
  int i = 8;

  for (pos = 0; pos < len; pos++) {
    crc ^= (Uint16)buf[pos];          // XOR byte into least sig. byte of crc

    for (i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

void modbus_holding_register_init(Uint16 * holding_registers, Uint8 length)
{
  Uint16 i = 0; 
  Uint16 eeprom_status = 0;
  eeprom_status = eeprom_read_two_bytes(0xfffe);
  if (eeprom_status != 0xAAAA){
    for (i=0; i<8; i++){
      holding_registers[i] = 25500;
    }
    for (; i<length; i++){
      holding_registers[i] = 0;
    }
    holding_registers[25] = 0xFF; //enable relays
    holding_registers[27] = 0xFF;
    holding_registers[44] = 0xFF;
    holding_registers[35] = 20;
    holding_registers[36] = 30;
    //holding_registers[33] = 0x07; //set DT to manual
    holding_registers[39] = BUILD_YEAR;
    holding_registers[40] = BUILD_MONTH;
    holding_registers[41] = BUILD_DAY;

    //setting default setpoints
    holding_registers[46] = 800;
    holding_registers[47] = 1000;
    holding_registers[48] = 1500;
    holding_registers[49] = 1500;
    //      FreeCool
    holding_registers[50] = 1800;
    holding_registers[51] = 2500;
    holding_registers[52] = 3000;
    holding_registers[53] = 2000;
    holding_registers[54] = 3000;
    holding_registers[55] = 3500;
    holding_registers[56] = 3000;

    //      Damper
    holding_registers[57] = 500;
    holding_registers[58] = 1200;
    holding_registers[59] = 3000;
    holding_registers[60] = 1000;
    holding_registers[61] = 2000;
    holding_registers[62] = 3500;
    holding_registers[63] = 3000;

    //      compressor on
    holding_registers[64] = 3500;
    holding_registers[65] = 3700;
    holding_registers[66] = 3000;
    holding_registers[67] = 3000;
    //      compressor on
    holding_registers[68] = 4000;
    holding_registers[69] = 3900;

    for (i=0; i<length; i++){
      eeprom_store_two_bytes(holding_registers[i],i);
      Task_sleep(5);
    }
    eeprom_store_two_bytes(0xAAAA,0xfffe);
  } else {
    for (i=0; i<length; i++){
      holding_registers[i] = eeprom_read_two_bytes(i);
    }
  }
}
