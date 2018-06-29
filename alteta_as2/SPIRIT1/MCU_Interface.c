#include "MCU_Interface.h"
#include "main.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MCU_Interface.h"
#include "SPIRIT_Types.h"
#include "SPIRIT_Commands.h"

#define SPIRIT_SS_PORT                PORTB
#define SPIRIT_SS_PIN                 2
#define SPIRIT_SS_UP                  SPIRIT_SS_PORT|=(1<<SPIRIT_SS_PIN)
#define SPIRIT_SS_DOWN                SPIRIT_SS_PORT&=~(1<<SPIRIT_SS_PIN)

#define SPIRTT_READ_PREFIX            0x01
#define SPIRIT_WRITE_PREFIX           0x00
#define SPIRIT_CMD_PREFIX             0x80


StatusBytes SdkEvalSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
	StatusBytes     s_Spirit_Status;

	uint16_t        status=0;
	uint8_t         data;
	uint16_t        i=0;
	
	SPIRIT_SS_DOWN;
	
	data=spi_m(SPIRTT_READ_PREFIX);
	status|=((uint16_t)data)<<8;
	data=spi_m(cRegAddress);
	status|=((uint16_t)data)<<0;
	
	for (i=0; i<cNbBytes; i++)
	{
		data=spi_m(0x55);
		pcBuffer[i]=data;
	}
	
	SPIRIT_SS_UP;
	
	memcpy(&s_Spirit_Status,&status,2);

	return s_Spirit_Status;
}

StatusBytes SdkEvalSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{

	StatusBytes     s_Spirit_Status;
	uint16_t        status=0;
	uint8_t         data;
	uint16_t        i=0;
	
	SPIRIT_SS_DOWN;
	
	data=spi_m(SPIRIT_WRITE_PREFIX);
	status|=((uint16_t)data)<<8;
	data=spi_m(cRegAddress);
	status|=((uint16_t)data)<<0;
	
	for (i=0; i<cNbBytes; i++)
	{
		data=spi_m(pcBuffer[i]);
	}
	
	SPIRIT_SS_UP;
	
	memcpy(&s_Spirit_Status,&status,2);
	
	return s_Spirit_Status;

}


StatusBytes SdkEvalSpiCommandStrobes(uint8_t cCommandCode)
{

	StatusBytes     s_Spirit_Status;
	uint16_t        status=0;
	uint8_t         data;
	
	SPIRIT_SS_DOWN;
	
	data=spi_m(SPIRIT_CMD_PREFIX);
	status|=((uint16_t)data)<<8;
	data=spi_m(cCommandCode);
	status|=((uint16_t)data)<<0;
	
	SPIRIT_SS_UP;
	
	memcpy(&s_Spirit_Status,&status,2);
	return s_Spirit_Status;


}





