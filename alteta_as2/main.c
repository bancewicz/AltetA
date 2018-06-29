#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <atmel_start.h>
#include "main.h"
#include "terminal.h"
#include "flags.h"

//////////////////////////////////////////////////////GLOBALS////////////////////////////////////////////////////////////

struct_flags    flags1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void banner (void)
{
	printf("\r\nALTETA-AS2 generation project for SPIRIT1 v.1.001");
	printf("\r\n-------------------------------------------------");
}

ISR(USART_RX_vect)
{
	char data;
	data=UDR0;
	terminal_proc (data);
}


/*
unsigned char spi_m(unsigned char data)
{
	
	unsigned char d=data;
	unsigned char dd=0;
	int ii=0xff;


//	DDRB|=(1<<5);    //sck
//	DDRB|=(1<<3);    //mosi
//	DDRB&=~(1<<4);   //miso
	
	PORTB&=~(1<<5);  //sck down
		
	for (int i= 8; i>0; i--)	
	{
		

		if (d&0x80)  PORTB|= (1<<3);
		else         PORTB&=~(1<<3);
		
		
		PORTB|=(1<<5); 
		ii=0x1; while (ii--);

		PORTB&=~(1<<5);
		ii=0x1; while (ii--);

		if (PINB&(1<<4))  dd|=(0x80>>8-i);
	
		d<<=1;
		
	}
	
	return dd;
	
}

*/

unsigned char spi_m(unsigned char data)
{
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	SPSR|=(1<<SPIF);
	return SPDR;
}


void spi_init(void)
{
	/* Set MOSI and SCK output, all others input */
	
	
	DDRB=   (1<<2);	  //SS	
	PORTB|= (1<<2);
	
	DDRB|= (1<<5);    //SCK
	DDRB|= (1<<3);    //MOSI
	DDRB&=~(1<<4);    //MISO
	
	PRR &= ~(1 << PRSPI);
	
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(0<<SPI2X)|(0<<SPR1)|(0<<SPIE)|(0<<CPOL)|(0<<CPHA);
	//CPOL
	//CPHA
	//SPI2X
	//SPR1
	//SPR0
	
	SPSR|=(1<<SPIF);
}




int main(void)
{

	int tact=0;
	unsigned short reg=0;
	unsigned short stat=0;
	unsigned short adr=0;
	unsigned char  rad=0;
	unsigned char  data=0;
	
	unsigned char stx[4];
	unsigned char srx[4];
	int ii=0xff;
	
	cli();	
	
	
	static FILE mystdout = FDEV_SETUP_STREAM(USART_0_write, NULL, _FDEV_SETUP_WRITE);
	stdout=&mystdout;
	
	atmel_start_init();
	banner();
	
	DDRC|=(1<<5);
    PORTC|= (1<<5); 
	ii=0xff; while (ii--);
    PORTC&=~(1<<5); 

/*
	DDRB=(1<<2);
	PORTB|=(1<<2);
	
	DDRB|=(1<<5);    //sck
	DDRB|=(1<<3);    //mosi
	DDRB&=~(1<<4);   //miso
*/

	spi_init();	
	
//SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
//SPSR=(0<<SPI2X);
	
	sei();

		PORTB|=(1<<2);
		PORTB&=~(1<<2);
		//data=SPI_0_exchange_byte((uint8_t)adr>>8&0xff);
		//data=SPI_0_exchange_byte(0x01); while (SPI_0_status_busy());
		data=spi_m(0x01);
		stat|=(unsigned short)data; 
		stat<<=8;
	
		//data=SPI_0_exchange_byte((uint8_t)adr&0xff);
		//data=SPI_0_exchange_byte(rad);  while (SPI_0_status_busy());
		data=spi_m(rad);
		stat|=(unsigned short)data;
	
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
		reg<<=8;
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
		PORTB|=(1<<2);				


		PORTB|=(1<<2);
		PORTB&=~(1<<2);
		//data=SPI_0_exchange_byte((uint8_t)adr>>8&0xff);
		//data=SPI_0_exchange_byte(0x01); while (SPI_0_status_busy());
		data=spi_m(0x01);
		stat|=(unsigned short)data;
		stat<<=8;
		
		//data=SPI_0_exchange_byte((uint8_t)adr&0xff);
		//data=SPI_0_exchange_byte(rad);  while (SPI_0_status_busy());
		data=spi_m(rad);
		stat|=(unsigned short)data;
		
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
		reg<<=8;
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
		PORTB|=(1<<2);


	terminal_init ();

	/* Replace with your application code */
	while (1) {
		
		
		      ii=0xffff; while (ii--);
			  
			  if (RXFLAG) 
			  {
				terminal_proc_cmd();
			  }
			  else
			  {
				  ii=0xffff; while (ii--);
			  }
				
			continue;	
		
		for (int i=0; i<0xfff; i++)
		{
			for (int j=0; j<0x4ff; j++)
			{
				
				j=j;
				
			}
			
		}
		
		//printf("\r\n--> tact %d", tact++);
		
		
		adr=0x0100|rad;
		stat=0;
		reg=0;
		
		printf("\r\n--> adr = %d", rad);
		
		PORTB|=(1<<2);
		PORTB&=~(1<<2);
	
		//data=SPI_0_exchange_byte((uint8_t)adr>>8&0xff);
		//data=SPI_0_exchange_byte(0x01); while (SPI_0_status_busy());
		data=spi_m(0x01);
		stat|=(unsigned short)data; 
		stat<<=8;
	
		//data=SPI_0_exchange_byte((uint8_t)adr&0xff);
		//data=SPI_0_exchange_byte(rad);  while (SPI_0_status_busy());
		data=spi_m(rad);
		stat|=(unsigned short)data;
	
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
		reg<<=8;
		//data=SPI_0_exchange_byte(0x55);  while (SPI_0_status_busy());
		data=spi_m(0xaa);
		reg|=(unsigned short)data;
	
	
	/*	
		memset(stx,0,4);
		stx[0]=0x01;
		stx[1]=rad;
		stx[2]=0x55;
		stx[3]=0x55;
		
		SPI_0_exchange_block(stx,4); while (SPI_0_status_busy());
	*/
				
		PORTB|=(1<<2);				
				
		//stat=*((unsigned short*)(&stx[0]));
		//reg= *((unsigned short*)(&stx[2]));		
				
		printf(" stat = %04X", stat);
		printf(" reg  = %04X", reg);
		
		
		rad+=1;

	}
}
