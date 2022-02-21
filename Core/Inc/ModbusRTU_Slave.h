/*
 * Modbus_Lib.h
 *
 *  Created on: Feb 10, 2022
 *      Author: Ümit Can Güveren
 */

#ifndef INC_MODBUSRTU_SLAVE_H_
#define INC_MODBUSRTU_SLAVE_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"


#define SLAVEID    1
#define NUMBER_OF_REGISTER 10
#define NUMBER_OF_COIL 30
#define BUFFERSIZE (NUMBER_OF_REGISTER * 2 + 5)

uint16_t ModbusRegister[NUMBER_OF_REGISTER];
bool ModbusCoil[NUMBER_OF_COIL];

enum
{
	ReadCoil = 0x01,
	ReadHoldingRegister = 0x03,
	WriteSingleCoil = 0x05,
	WriteSingleRegister = 0x06,
	WriteMultipleCoils = 0x0F,
	WriteMultipleResisters = 0x10,
};


extern uint8_t uartRxData;
extern uint8_t DataCounter;
extern uint8_t RxInterruptFlag;
extern uint8_t uartTimeCounter;
extern uint8_t uartPacketComplatedFlag;
extern char ModbusRx[BUFFERSIZE];

/* Receiver functions */
void uartDataHandler(void);
void uartTimer(void);
void transmitDataMake(char *msg, uint8_t Lenght);


/* Modbus Functions */
void makePacket_01(char *msg, uint8_t Lenght);
void makePacket_03(char *msg, uint8_t Lenght);
void makePacket_05(char *msg, uint8_t Lenght);
void makePacket_06(char *msg, uint8_t Lenght);
void makePacket_15(char *msg, uint8_t Lenght);
void makePacket_16(char *msg, uint8_t Lenght);


void sendMessage(char *msg, uint8_t len);
uint8_t findByte(int16_t NumberOfCoil);
uint16_t MODBUS_CRC16(char *buf, uint8_t len );



#endif /* INC_MODBUSRTU_SLAVE_H_ */
