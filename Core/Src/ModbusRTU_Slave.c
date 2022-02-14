/*
 * Modbus_Lib.c
 *
 *  Created on: Feb 10, 2022
 *      Author: DELL
 */
#include <ModbusRTU_Slave.h>
#include "main.h"

uint8_t uartRxData;
uint8_t DataCounter;
uint8_t RxCpltCallbackFlag;
uint8_t uartTimeoutcounter;
uint8_t uartReceiveComplatedFlag;

char ModbusRx[BUFFERSIZE];
char tempModbusRx[BUFFERSIZE];
char ModbusTx[BUFFERSIZE];
uint16_t rxCRC;
//uint16_t RegAddress;
//uint16_t NumberOfReg;

extern UART_HandleTypeDef huart2;


void sendMessage(char *msg, uint8_t len)
{
	HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg, len);

}


static void makePacket_01(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, NumberCoils, NumberByte,CRCValue;
	RegAddress = (msg[2] << 8) | (msg[3]);	//Okunacak bobinlerin başlangıç adresi.
	NumberCoils = (msg[4] << 8) | (msg[5]); //Okunacak bobin adedi.
	NumberByte = (NumberCoils / 9) + 1;

	uint8_t tempCoil[NumberByte];
	uint8_t CoilCount, i = 0, ByteCount = 0;

	ModbusTx[0] = msg[0];
	ModbusTx[1] = msg[1];
	ModbusTx[2] = NumberByte;


	for(CoilCount = 0; CoilCount < NumberCoils; CoilCount++)
	{
			if( CoilCount % 8 == 0 && CoilCount != 0)
			{
				ByteCount++;
				i = 0;
			}
			tempCoil[ByteCount] |= (ModbusCoil[RegAddress + CoilCount] << i);
			i++;
	}

	for(i = 0; i < ByteCount + 1; i++)
	{
		ModbusTx[3 + i] = tempCoil[i];
	}

	CRCValue = MODBUS_CRC16(ModbusTx, 3 + i);
	ModbusTx[3 + i] = (CRCValue & 0x00FF);
	ModbusTx[4 + i] = (CRCValue >> 8);
	sendMessage(ModbusTx, 5 + i);
	memset(tempCoil, 0, 8 * ByteCount);
}

static void makePacket_03(char *msg, uint8_t Lenght)
{
	uint8_t i, m = 0;

	uint16_t RegAddress = 0;
	uint16_t NumberOfReg = 0;
	uint16_t CRCValue;

	RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfReg = (msg[4] << 8) | (msg[5]);
	ModbusTx[0] = msg[0];
	ModbusTx[1] = msg[1];
	ModbusTx[2] = (NumberOfReg * 2);

	for(i = 0; i < NumberOfReg * 2; i += 2)
	{
		ModbusTx[3 + i] = (uint8_t)(ModbusRegister[RegAddress + m] >> 8);
		ModbusTx[4 + i] = (uint8_t)(ModbusRegister[RegAddress + m] & 0x00FF);
		m++;
	}

	//CRC Calculate
	CRCValue = MODBUS_CRC16(ModbusTx, 3 + (NumberOfReg * 2 ));
	ModbusTx[4 + (NumberOfReg * 2 )] = (CRCValue >> 8);
	ModbusTx[3 + (NumberOfReg * 2 )] = (CRCValue & 0x00FF);
	/********************************************************/
	sendMessage(ModbusTx, 5 + (NumberOfReg * 2 ));
}


static void makePacket_05(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusCoil[RegAddress] = (bool)RegValue;

	sendMessage(msg, Lenght);
}

static void makePacket_06(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, RegValue;
	RegAddress = (msg[2] << 8) | (msg[3]);
	RegValue = (msg[4] << 8) | (msg[5]);

	ModbusRegister[RegAddress] = RegValue;
	sendMessage(msg, Lenght);

}

static void makePacket_15(char *msg, uint8_t Lenght)
{
	uint16_t RegAddress, NumberOfByte, NumberOfCoils, CRCValue;
	uint8_t i,m,k;
	RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfCoils = (msg[4] << 8) | (msg[5]);
	NumberOfByte = msg[6];

	for(i = 0; i < NumberOfCoils; i++)
	{
		ModbusCoil[i] = (msg[7 + m] & (0x01 << k)) ;

		k++;
		if(k % 8 == 0 && k != 0)
		{
			m++;
			k = 0;
		}
	}

	memcpy(ModbusTx, msg, 6);

	CRCValue = MODBUS_CRC16(ModbusTx, 6);
	ModbusTx[6] = (CRCValue & 0x00FF);
	ModbusTx[7] = (CRCValue >> 8);
	sendMessage(ModbusTx, 8);
}



static void makePacket_16(char *msg, uint8_t Lenght)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	uint16_t RegAddress, NumberOfReg, CRCValue;
	uint8_t i,m = 0;
	RegAddress = (msg[2] << 8) | (msg[3]);
	NumberOfReg = (msg[4] << 8) | (msg[5]);

	for(i = 0; i < NumberOfReg; i++)
	{
		ModbusRegister[RegAddress + i] = (uint16_t)((uint16_t)msg[7 + m] << 8) | (msg[8 + m]);
		m += 2;
	}

    memcpy(ModbusTx, msg, 6);

	CRCValue = MODBUS_CRC16(ModbusTx, 6);
	ModbusTx[6] = (CRCValue & 0x00FF);
	ModbusTx[7] = (CRCValue >> 8);
	sendMessage(ModbusTx, 8);
}


void transmitDataMake(char *msg, uint8_t Lenght)
{
	switch(msg[1])
	{
	case ReadCoil:
		makePacket_01(msg, Lenght);
		break;

	case ReadHoldingRegister:
		makePacket_03(msg, Lenght);
		break;

	case WriteSingleRegister:
		makePacket_06(msg, Lenght);
		break;

	case WriteSingleCoil:
		makePacket_05(msg, Lenght);
		break;

	case WriteMultipleCoils:
		makePacket_15(msg, Lenght);
		break;

	case WriteMultipleResisters:
		makePacket_16(msg, Lenght);
		break;
	}

}

void uartDataHandler(void)
{
	uint8_t tempCounter;
	uint16_t CRCValue;

	if(uartReceiveComplatedFlag == SET)     //alma islemi tammalandi
	{
		uartReceiveComplatedFlag = RESET;
	    memcpy(tempModbusRx, ModbusRx, DataCounter + 1);
	    tempCounter = DataCounter;
		DataCounter = 0;
		memset(ModbusRx, 0, BUFFERSIZE);
		memset(ModbusTx, 0, BUFFERSIZE);

		//CRC dogruysa slave gereken datayı gönderir.
		CRCValue = MODBUS_CRC16(tempModbusRx, tempCounter - 2); //CRC Hesaplama
		rxCRC = (tempModbusRx[tempCounter -1] << 8) | (tempModbusRx[tempCounter - 2]); //Alınan CRC değeri rxCRC değine atanır.

		if(rxCRC == CRCValue && tempModbusRx[0] == SLAVEID) 	//İki değer eşitse data oluşturulur ve geri gönderilir.
		{
			transmitDataMake(&tempModbusRx[0], tempCounter);
		}

	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	RxCpltCallbackFlag = SET; // data geldi bayragi
	ModbusRx[DataCounter++] = uartRxData; //gelen datayÄ± sÄ±ra ile buffera alÄ±yor
	if(DataCounter >= BUFFERSIZE)
	{
		DataCounter  = 0;
	}

	HAL_UART_Receive_IT(&huart2 , &uartRxData , 1); // yeni data iÃ§in kesmeyi tekrar kur
	uartTimeoutcounter = 0; // her data geldiÄŸinde timeouttu sÄ±fÄ±rla

}

// This function called in the systick timer
void uartInsideTimer(void)
{
	if(RxCpltCallbackFlag == SET)
	{
		if(uartTimeoutcounter++ > 100) //uarttÄ±meout kÄ±satÄ±lacak test edilip
		{

			RxCpltCallbackFlag = RESET; //flagleri temizle
			uartTimeoutcounter = 0;
			uartReceiveComplatedFlag = SET; // data alÄ±mÄ± bitti
		}
	}
}


uint16_t MODBUS_CRC16(char *buf, uint8_t len )
{
	static const uint16_t table[2] = { 0x0000, 0xA001 };
	uint16_t crc = 0xFFFF;
	unsigned int i = 0;
	char bit = 0;
	unsigned int xor = 0;

	for( i = 0; i < len; i++ )
	{
		crc ^= buf[i];

		for( bit = 0; bit < 8; bit++ )
		{
			xor = crc & 0x01;
			crc >>= 1;
			crc ^= table[xor];
		}
	}

	return crc;
}

