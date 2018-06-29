#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "terminal.h"
#include <avr/io.h>


#include "main.h"
#include "SPIRIT1/MCU_Interface.h"
#include "SPIRIT1/SPIRIT_Types.h"
#include "SPIRIT1/SPIRIT_Calibration.h"
#include "SPIRIT1/SPIRIT_Commands.h"
#include "SPIRIT1/SPIRIT_Management.h"
#include "SPIRIT1/SPIRIT_Csma.h"
#include "SPIRIT1/SPIRIT_Radio.h"
#include "SPIRIT1/SPIRIT_AES.h"
#include "SPIRIT1/SPIRIT_DirectRF.h"
#include "SPIRIT1/SPIRIT_General.h"
#include "SPIRIT1/SPIRIT_Gpio.h"
#include "SPIRIT1/SPIRIT_Irq.h"
#include "SPIRIT1/SPIRIT_LinearFIFO.h"
#include "SPIRIT1/SPIRIT_PktBasic.h"
#include "SPIRIT1/SPIRIT_PktCommon.h"
#include "SPIRIT1/SPIRIT_PktMbus.h"
#include "SPIRIT1/SPIRIT_PktStack.h"
#include "SPIRIT1/SPIRIT_Qi.h"
#include "SPIRIT1/SPIRIT_Timer.h"
#include "SPIRIT1/SPIRIT_Config.h"

void help (void)
{
   int i=0;    
   printf ("\r\ncommands list:"); 
   for (i=0; i<CMDLISTVAL; i++)
   {       
       if (commands[i].function==NULL) break; 
       printf ("\r\n%s",commands[i].command);
   }
 
}


void spirit_reg_read(void)
{
	uint8_t     addr=0;
	uint8_t     val[2];
	
	if (cmdargs[0]==NULL) ERRORARGUMENT;
	addr=atoi(cmdargs[0]);
	
	if (addr>255) ERRORARGUMENT;
	
	SdkEvalSpiReadRegisters(addr,2,(uint8_t*)&val);
	
	printf ("\r\n-RES spirit1 reg [%02Xh] = %02X%02Xh",addr,val[0], val[1]);

}


void spirit_reg_write(void)
{
	uint8_t     addr=0;
	uint16_t    val;
	uint8_t     vval[2];
	
	if (cmdargs[0]==NULL) ERRORARGUMENT;
	addr=atoi(cmdargs[0]);
	if (addr>255) ERRORARGUMENT;
	
	if (cmdargs[1]==NULL) ERRORARGUMENT;
	sscanf (cmdargs[1],"%04x",&val);
	
	if (val>65535) ERRORARGUMENT;
	
	vval[0]=(uint8_t)(val>>8)&0xff;
	vval[1]=(uint8_t)(val&0xff);
	
	SdkEvalSpiWriteRegisters(addr,2,(uint8_t*)&vval);

}

void spirit_cmd_write(void)
{
	uint8_t cmd=0;

	if (cmdargs[0]==NULL) ERRORARGUMENT;
	cmd=atoi(cmdargs[0]);
	if (cmd>255) ERRORARGUMENT;
	
	SdkEvalSpiCommandStrobes(cmd);

}


void spirit_status_view(void)
{
	int stat=0;
	
	uint8_t       buf[2];
	uint16_t      status=0;
	
	g_xStatus=SdkEvalSpiReadRegisters(0,2,buf);
	status=*((uint16_t*)(&g_xStatus));
	
	printf ("\r\n-I g_xStatus = %04Xh", status);
	
	printf ("\r\ng_xStatus.ANT_SELECT=%02Xh",g_xStatus.ANT_SELECT);
	printf ("\r\ng_xStatus.ERROR_LOCK=%02Xh",g_xStatus.ERROR_LOCK);
	printf ("\r\ng_xStatus.MC_STATE=%02Xh",g_xStatus.MC_STATE);
	
	stat=(status&0xff)>>1;
	
	//switch (g_xStatus.MC_STATE)
	switch (stat)
	{
		
		case  MC_STATE_STANDBY:
		printf ("<STANDBY>");
		break;

		
		case    MC_STATE_LOCKWON:
		printf ("<LOCKWON>");
		break;
		
		case    MC_STATE_READY:
		printf ("<READY>");
		break;

		case    MC_STATE_PROTOCOL:
		printf ("<PROTOCOL>");
		break;

		case    MC_STATE_LOCK:
		printf ("<LOCK>");
		break;
		
		case    MC_STATE_XO_SETTLING:
		printf ("<XO_SETTLING>");
		break;

		case    MC_STATE_RX:
		printf ("<RX>");
		break;

		case    MC_STATE_SLEEP:
		printf ("<SLEEP>");
		break;

		case    MC_STATE_PM_SETUP:
		printf ("<PM_SETUP>");
		break;




		
		case    MC_STATE_SYNTH_CALIBRATION:
		printf ("<SYNTH>");
		break;
		
		case    MC_STATE_SYNTH_SETUP:
		printf ("<SYNT_SETUP>");
		break;
		
		case    MC_STATE_TX:
		printf ("<TX>");
		break;
		
		
		default:
		printf("<UNKNOWN>");
	}
	
	printf ("\r\ng_xStatus.RX_FIFO_EMPTY=%02Xh",g_xStatus.RX_FIFO_EMPTY);
	printf ("\r\ng_xStatus.TX_FIFO_FULL=%02Xh",g_xStatus.TX_FIFO_FULL);
	printf ("\r\ng_xStatus.XO_ON=%02Xh",g_xStatus.XO_ON);
}


void spirit_command_ready(void)
{
	
uint8_t state;
int tout=0;
	
SpiritSpiCommandStrobes(COMMAND_READY);
   do{
    SpiritSpiReadRegisters(0xC1, 1, &state);
	tout++;
		if (tout>0xefff)
		{
			printf("\r\n-ERR timeout");
			return;
		}
  }while((state&0xFE) != 0x06); /* wait until READY (MC_STATE = 0x03 <<1) */
  
	spirit_status_view();
}

void spirit_command_standby(void)
{
	SpiritSpiCommandStrobes(COMMAND_STANDBY);
	spirit_status_view();
}


void spirit_command_lock_tx(void)
{
	
uint8_t state;
int tout=0;
	
  SpiritSpiCommandStrobes(COMMAND_LOCKTX);
  do{
	  SpiritSpiReadRegisters(0xC1, 1, &state);
	  tout++;
		if (tout>0xefff)
		{
			printf("\r\n-ERR timeout");
			return;
		}
	  }while((state&0xFE) != 0x1E); // wait until LOCK (MC_STATE = 0x0F <<1) 
	
	spirit_status_view();	
}


void spirit_command_lock_rx(void)
{
 uint8_t state;
 int tout=0;

 SpiritSpiCommandStrobes(COMMAND_LOCKRX);
 do{
	SpiritSpiReadRegisters(0xC1, 1, &state);
	tout++;
		if (tout>0xefff)
		{
			printf("\r\n-ERR timeout");
			return;
		}
	}while((state&0xFE) != 0x1E); // wait until LOCK (MC_STATE = 0x0F <<1) 
	
	spirit_status_view();
	
}

void spirit_command_sleep(void)
{
	
	SpiritSpiCommandStrobes(COMMAND_SLEEP);
	spirit_status_view();
	
}

void spirit_command_lock(void)
{
	
	
}

void spirit_command_disable(void)
{
	
	PORTC|= (1<<5);
}

void spirit_command_enable(void)
{
	
	PORTC&=~(1<<5);
}


void spirit_command_reset(void)
{
	
	SpiritSpiCommandStrobes(COMMAND_SRES);
	spirit_status_view();

}











//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/**

The SPI interface is platform dependent, this means that it should be implemented according to the used hardware.

At least the 3 functions:
        SpiritSpiWriteRegisters(uint8_t address, uint8_t n_regs, uint8_t* buffer)
        SpiritSpiReadRegisters(uint8_t address, uint8_t n_regs, uint8_t* buffer)
        SpiritSpiCommandStrobes(uint8_t cmd_code)
must be implemented.

An example of implementation (for the SDK_EVAL motherboards of the SPIRIT kit) can be found into the file: Firmware/STM32L/SDK_Eval_STM32L/Source/src/SDK_EVAL_Spi_Driver.c
It is advisable to implement also the 2 FIFO functions to read and write (no reference in this template code but needed in applications using the FIFO of SPIRIT).

These functions must be called in the following order:

 SpiritBaseConfiguration();
 SpiritVcoCalibration();

**/

/* list of the command codes of SPIRIT1 */
#define	COMMAND_TX                                          ((uint8_t)(0x60)) /*!< Start to transmit; valid only from READY */
#define	COMMAND_RX                                          ((uint8_t)(0x61)) /*!< Start to receive; valid only from READY */
#define	COMMAND_READY                                       ((uint8_t)(0x62)) /*!< Go to READY; valid only from STANDBY or SLEEP or LOCK */
#define	COMMAND_STANDBY                                     ((uint8_t)(0x63)) /*!< Go to STANDBY; valid only from READY */
#define	COMMAND_SLEEP                                       ((uint8_t)(0x64)) /*!< Go to SLEEP; valid only from READY */
#define	COMMAND_LOCKRX                                      ((uint8_t)(0x65)) /*!< Go to LOCK state by using the RX configuration of the synth; valid only from READY */
#define	COMMAND_LOCKTX                                      ((uint8_t)(0x66)) /*!< Go to LOCK state by using the TX configuration of the synth; valid only from READY */
#define	COMMAND_SABORT                                      ((uint8_t)(0x67)) /*!< Force exit form TX or RX states and go to READY state; valid only from TX or RX */
#define	COMMAND_SRES                                        ((uint8_t)(0x70)) /*!< Reset of all digital part, except SPI registers */
#define	COMMAND_FLUSHRXFIFO                                 ((uint8_t)(0x71)) /*!< Clean the RX FIFO; valid from all states */
#define	COMMAND_FLUSHTXFIFO                                 ((uint8_t)(0x72)) /*!< Clean the TX FIFO; valid from all states */


/* This is the function that initializes the SPIRIT with the configuration 
that the user has exported using the GUI */
void SpiritBaseConfiguration(void)
{
  uint8_t tmp[6];

  /* Be sure that the registers config is default */
  SpiritSpiCommandStrobes(COMMAND_SRES);

  /* Extra current in after power on fix.
     In some samples, when a supply voltage below 2.6 V is applied to SPIRIT1 from a no power condition,
     an extra current is added to the typical current consumption.
     With this sequence, the extra current is erased.
  */
  tmp[0]=0xCA;SpiritSpiWriteRegisters(0xB2, 1, tmp); 
  tmp[0]=0x04;SpiritSpiWriteRegisters(0xA8, 1, tmp); 
  SpiritSpiReadRegisters(0xA8, 1, tmp);
  tmp[0]=0x00;SpiritSpiWriteRegisters(0xA8, 1, tmp);

  tmp[0] = 0x31; /* reg. IF_OFFSET_ANA (0x07) */
  tmp[1] = 0x06; /* reg. SYNT3 (0x08) */
  tmp[2] = 0x42; /* reg. SYNT2 (0x09) */
  tmp[3] = 0x76; /* reg. SYNT1 (0x0A) */
  tmp[4] = 0x21; /* reg. SYNT0 (0x0B) */
  tmp[5] = 0x01; /* reg. CH_SPACE (0x0C) */
  SpiritSpiWriteRegisters(0x07, 6, tmp);
  tmp[0] = 0x17; /* reg. PA_POWER[8] (0x10) */
  SpiritSpiWriteRegisters(0x10, 1, tmp);
  tmp[0] = 0xC8; /* reg. AFC2 (0x1E) */
  SpiritSpiWriteRegisters(0x1E, 1, tmp);
  tmp[0] = 0x62; /* reg. AGCCTRL1 (0x25) */
  SpiritSpiWriteRegisters(0x25, 1, tmp);
  tmp[0] = 0x15; /* reg. ANT_SELECT_CONF (0x27) */
  SpiritSpiWriteRegisters(0x27, 1, tmp);
  tmp[0] = 0x3F; /* reg. PCKTCTRL2 (0x32) */
  tmp[1] = 0x30; /* reg. PCKTCTRL1 (0x33) */
  SpiritSpiWriteRegisters(0x32, 2, tmp);
  tmp[0] = 0x41; /* reg. PCKT_FLT_OPTIONS (0x4F) */
  tmp[1] = 0x40; /* reg. PROTOCOL[2] (0x50) */
  tmp[2] = 0x01; /* reg. PROTOCOL[1] (0x51) */
  SpiritSpiWriteRegisters(0x4F, 3, tmp);
  tmp[0] = 0x00; /* reg. RCO_VCO_CALIBR_IN[1] (0x6E) */
  tmp[1] = 0x00; /* reg. RCO_VCO_CALIBR_IN[0] (0x6F) */
  SpiritSpiWriteRegisters(0x6E, 2, tmp);
  tmp[0] = 0xA0; /* reg. SYNTH_CONFIG[0] (0x9F) */
  SpiritSpiWriteRegisters(0x9F, 1, tmp);
  tmp[0] = 0x25; /* reg. VCO_CONFIG (0xA1) */
  SpiritSpiWriteRegisters(0xA1, 1, tmp);
  tmp[0] = 0x35; /* reg. DEM_CONFIG (0xA3) */
  SpiritSpiWriteRegisters(0xA3, 1, tmp);

  /* VCO unwanted calibration workaround. 
     With this sequence, the PA is on after the eventual VCO calibration expires.
  */
  tmp[0]=0x22;SpiritSpiWriteRegisters(0xBC, 1, tmp);

}

/* This is a VCO calibration routine used to recalibrate the VCO of SPIRIT1 in a safe way.
 IMPORTANT: It must be called from READY state. */
void SpiritVcoCalibration(void)
{
  uint8_t tmp[4];
  uint8_t cal_words[2];
  uint8_t state;


    
  SpiritSpiReadRegisters(0x9E, 1, tmp);
  tmp[0] |= 0x80;
  SpiritSpiWriteRegisters(0x9E, 1, tmp); /* REFDIV bit set (to be restored) */

  /* As a consequence we need to double the SYNT word to generate the target frequency */
  tmp[0] = 0x0C;
  tmp[1] = 0x84;
  tmp[2] = 0xEC;
  tmp[3] = 0x41;
  SpiritSpiWriteRegisters(0x08, 4, tmp);


  tmp[0] = 0x25; SpiritSpiWriteRegisters(0xA1,1,tmp); /* increase VCO current (restore to 0x11) */
  
  SpiritSpiReadRegisters(0x50,1,tmp);
  tmp[0] |= 0x02; 
  SpiritSpiWriteRegisters(0x50,1,tmp); /* enable VCO calibration (to be restored) */
  
  SpiritSpiCommandStrobes(COMMAND_LOCKTX);
  do{
    SpiritSpiReadRegisters(0xC1, 1, &state);
  }while((state&0xFE) != 0x1E); /* wait until LOCK (MC_STATE = 0x0F <<1) */
  SpiritSpiReadRegisters(0xE5, 1, &cal_words[0]); /* calib out word for TX */
  
  SpiritSpiCommandStrobes(COMMAND_READY);
   do{
    SpiritSpiReadRegisters(0xC1, 1, &state);
  }while((state&0xFE) != 0x06); /* wait until READY (MC_STATE = 0x03 <<1) */
  
  SpiritSpiCommandStrobes(COMMAND_LOCKRX);
  do{
    SpiritSpiReadRegisters(0xC1, 1, &state);
  }while((state&0xFE) != 0x1E); /* wait until LOCK (MC_STATE = 0x0F <<1) */
  SpiritSpiReadRegisters(0xE5, 1, &cal_words[1]); /* calib out word for RX */
  
  SpiritSpiCommandStrobes(COMMAND_READY);
   do{
    SpiritSpiReadRegisters(0xC1, 1, &state);
  }while((state&0xFE) != 0x06); /* wait until READY (MC_STATE = 0x03 <<1) */
  
  SpiritSpiReadRegisters(0x50,1,tmp);
  tmp[0] &= 0xFD; 
  SpiritSpiWriteRegisters(0x50,1,tmp); /* VCO calib restored to 0 */

  SpiritSpiReadRegisters(0x9E, 1, tmp);
  tmp[0] &= 0x7F;
  SpiritSpiWriteRegisters(0x9E, 1, tmp); /* REFDIV bit reset */

  
  tmp[0] = 0x06;
  tmp[1] = 0x42;
  tmp[2] = 0x76;
  tmp[3] = 0x21;
  SpiritSpiWriteRegisters(0x08, 4, tmp); /* SYNTH WORD restored */

  
  SpiritSpiWriteRegisters(0x6E,2,cal_words); /* write both calibration words */

}











/*
void spirit_init (void)
{

   uint16_t tmp=0;
   uint16_t reg=0;
   uint8_t  addr=0; 
   uint8_t base=0;  
   
#if SPIRIT1_MASTER_MODE==1   
   printf ("\r\nSPIRIT1 MODULE CONFIGURE MASTER MODE");  
#else    
   printf ("\r\nSPIRIT1 MODULE CONFIGURE SLAVE MODE");
#endif
  
//00
 
   base=8*ANA_FUNC_CONF1_BASE;
   addr=ANA_FUNC_CONF1_BASE; reg=0;
   printf ("\r\n--I reg[%d] ANA_FUNC_CONF[1] ANA_FUNC_CONF[0] configure ...", addr);  
   
   printf ("\r\n--I GM_CONF_MODE = %d",GM_CONF_MODE);
   reg|=GM_CONF_MODE<<base;                        
   
   printf ("\r\n--I BLD_LVL_MODE = %d",BLD_LVL_MODE);
   reg|=BLD_LVL_MODE<<base;
                          
//01   
   base=8*ANA_FUNC_CONF0_BASE;           
   
   printf ("--base = %d--", base);
   
   printf ("\r\n--I 24_26MHz_SELECTED = %d",SELECT_26_MHZ_FREQ);  
   
   //if (SELECT_26_MHZ_FREQ)          
                  
   printf ("--SELECT_24_26_MHZ_MASK=%d--",SELECT_24_26_MHZ_MASK);
   
    reg|=((uint16_t)(SELECT_24_26_MHZ_MASK)<<base);                                
   
   printf ("\r\n--I AES_MODULE_ENABLE = %d",AES_MODULE_ENABLE);
   reg|=AES_MASK&AES_MODULE_ENABLE<<base;
                                     
   printf ("\r\n--I  EXT_REF_ENABLE = %d", EXT_REF_ENABLE);
   if (EXT_REF_ENABLE) reg|=EXT_REF_MASK<<base; 
                                          
   printf ("\r\n--I  HIGH_POWER_MODE_ENABLE = %d", HIGH_POWER_MODE_ENABLE);
   if (HIGH_POWER_MODE_ENABLE) reg|=HIGH_POWER_MODE_MASK<<base; 
                          
   printf ("\r\n--I  BROWN_OUT_MASK = %d", BROWN_OUT_ENABLE);
   if (BROWN_OUT_ENABLE) reg|=BROWN_OUT_MASK<<base; 
   
   printf ("\r\n--I  BROWN_OUT_MASK = %d", BATTERY_LEVEL_ENABLE);
   if (BATTERY_LEVEL_ENABLE) reg|=BATTERY_LEVEL_MASK<<base; 
   
   printf ("\r\n--I  BROWN_OUT_MASK = %d", TEMPERATURE_SENSOR_ENABLE);
   if (TEMPERATURE_SENSOR_ENABLE) reg|=TEMPERATURE_SENSOR_MASK<<base; 
      
   write_register(addr,reg); 
   
   //sleep(1000);
   
   tmp=read_register(addr);
                  
   printf ("\r\n--I reg[%d] ANA_FUNC_CONF[1] ANA_FUNC_CONF[0] valset = %04X valread = %04X", reg, tmp);  
      
}


void spirit_write_cmd (void)
{
    uint8_t cmd=0;    

    if (cmdargs[0]==NULL) ERRORARGUMENT;
    cmd=atoi(cmdargs[0]);
    if (cmd>255) ERRORARGUMENT; 
                  
    printf (" --cmd = %02Xh--",cmd);
    write_cmd (cmd); 
    printf ("\r\n-RES spirit1 status = %04Xh [%02X]",spirit1_status, (spirit1_status&0xff)>>1);   
}

void spirit_write_reg (void)
{
    uint8_t     addr;
    uint16_t    val; 
    //char tmp[16];
        
    if (cmdargs[0]==NULL) ERRORARGUMENT; 
    addr=atoi(cmdargs[0]);    
    //printf ("--arg0=%s--",cmdargs[0]);
    
    //memset(tmp,0,16);
    //strcpy(tmp,cmdargs[0]);  
     
    //sscanf (tmp,"%h",&addr);
    
    //printf ("--addr=%02X--",addr);
                                   
    //addr=strtol(cmdargs[0],NULL,16);  
    
    if (addr>255) ERRORARGUMENT;
    if (cmdargs[1]==NULL) ERRORARGUMENT; 
    //val=atoi(cmdargs[1]);
    // val=strtol(cmdargs,NULL,16);
 
    sscanf (cmdargs[1],"%04x",&val);                      
//   printf ("--val=%04X  %s--",val, tmp);              
   
    if (val>65535) ERRORARGUMENT; 
    
    write_register (addr, val);  
    
    printf ("\r\n-RES spirit1 status = %04Xh [%04X] = ",spirit1_status,(spirit1_status&0xff)>>1); status_view();
}

void spirit_read_reg (void)
{
    uint8_t     addr=0;
    uint16_t    val=0;       
 
    if (cmdargs[0]==NULL) ERRORARGUMENT; 
    addr=atoi(cmdargs[0]); 
    //sscanf (cmdargs[0],"%02x",&addr);     
    
    if (addr>255) ERRORARGUMENT;   
    val=read_register(addr);
    
    printf ("\r\n-RES spirit1 reg [%02Xh] = %04Xh, status = %04Xh [%02X] = ",addr,val,spirit1_status,(spirit1_status&0xff)>>1); status_view();
        
}

void spirit_read_reg_all (void)
{
    
    uint8_t     addr=0;
    uint16_t    val=0;       
           
    for (addr=0; addr<32; addr++)
    {
       val=read_register(addr);
       printf ("\r\n-RES spirit1 reg [%02Xh] = %04Xh, status = %04Xh [%02X] = ",addr,val,spirit1_status,(spirit1_status&0xff>>1)); status_view();   
       sleep(100); 
    }

}

void spirit_shutdown (void)
{
     PORTC|= (1<<5);
}


void spirit_reset (void)
{
     PORTC|= (1<<5);
     sleep(100);
     PORTC&=~(1<<5);
}

void status_view(void)
{
    uint16_t status=0;
    status=(spirit1_status&0xff)>>1;         
                        
    switch(status)
    {
    
        case 0x40:
        printf("STANDBY");
        break;
        
        case 0x36:
        printf("SLEEP");
        break;

        case 0x03:
        printf("READY");
        break;

        case 0x0f:
        printf("LOCK");
        break;

        case 0x33:
        printf("RX");
        break;

        case 0x5f:
        printf("TX");
        break; 
        
        default:
        printf("UNKNOWN");
    
    }


}

*/



/*
//-------------------------------------TERMINAL FUNCTIONS SPIRIT1 LIB BASED----------------------------------------------------------


void spirit_reg_read(void)
{
	uint8_t     addr=0;
	uint8_t     val[2];
	
	if (cmdargs[0]==NULL) ERRORARGUMENT;
	addr=atoi(cmdargs[0]);
	
	if (addr>255) ERRORARGUMENT;
	
	SdkEvalSpiReadRegisters(addr,2,(uint8_t*)&val);
	
	printf ("\r\n-RES spirit1 reg [%02Xh] = %02X%02Xh",addr,val[0], val[1]);

}



void spirit_status_view(void)
{
      int stat=0;
        
      uint8_t       buf[2];                   
      uint16_t      status=0;       
      
      g_xStatus=SdkEvalSpiReadRegisters(0,2,buf); 
      status=*((uint16_t*)(&g_xStatus));    
      
      printf ("\r\n-I g_xStatus = %04Xh", status); 
      
      printf ("\r\ng_xStatus.ANT_SELECT=%02Xh",g_xStatus.ANT_SELECT);
      printf ("\r\ng_xStatus.ERROR_LOCK=%02Xh",g_xStatus.ERROR_LOCK);
      printf ("\r\ng_xStatus.MC_STATE=%02Xh",g_xStatus.MC_STATE);    
                                
      stat=(status&0xff)>>1; 
      
      //switch (g_xStatus.MC_STATE)
      switch (stat)   
      {
      
      case  MC_STATE_STANDBY: 
        printf ("<STANDBY>"); 
      break;

           
      case    MC_STATE_LOCKWON:           
        printf ("<LOCKWON>");  
      break;
       
      case    MC_STATE_READY:             
        printf ("<READY>");
      break;

      case    MC_STATE_PROTOCOL:          
        printf ("<PROTOCOL>");
      break;  

      case    MC_STATE_LOCK:              
        printf ("<LOCK>");
      break;  
      
      case    MC_STATE_XO_SETTLING:       
        printf ("<XO_SETTLING>");
      break;  

      case    MC_STATE_RX:                
        printf ("<RX>");
      break;  

      case    MC_STATE_SLEEP:             
        printf ("<SLEEP>");
      break;

      case    MC_STATE_PM_SETUP:          
        printf ("<PM_SETUP>"); 
      break;  




                 
      case    MC_STATE_SYNTH_CALIBRATION: 
        printf ("<SYNTH>");   
      break;  
        
      case    MC_STATE_SYNTH_SETUP:       
        printf ("<SYNT_SETUP>");
      break;  
        
      case    MC_STATE_TX:                
        printf ("<TX>");    
      break;  
        
        
        default:
        printf("<UNKNOWN>");
      }  
     
      printf ("\r\ng_xStatus.RX_FIFO_EMPTY=%02Xh",g_xStatus.RX_FIFO_EMPTY);
      printf ("\r\ng_xStatus.TX_FIFO_FULL=%02Xh",g_xStatus.TX_FIFO_FULL);
      printf ("\r\ng_xStatus.XO_ON=%02Xh",g_xStatus.XO_ON);
}


void spirit_reg_write(void)
{
    uint8_t     addr=0;
    uint16_t    val; 
    uint8_t     vval[2];      
 
    if (cmdargs[0]==NULL) ERRORARGUMENT; 
    addr=atoi(cmdargs[0]);     
    if (addr>255) ERRORARGUMENT;      
                           
    if (cmdargs[1]==NULL) ERRORARGUMENT; 
    sscanf (cmdargs[1],"%04x",&val);                      
   
    if (val>65535) ERRORARGUMENT; 
                                        
    vval[0]=(uint8_t)(val>>8)&0xff;
    vval[1]=(uint8_t)(val&0xff);
    
    SdkEvalSpiWriteRegisters(addr,2,(uint8_t*)&vval); 

}

void spirit_cmd_write(void)
{
    uint8_t cmd=0; 

    if (cmdargs[0]==NULL) ERRORARGUMENT; 
    cmd=atoi(cmdargs[0]);     
    if (cmd>255) ERRORARGUMENT;      
                              
    SdkEvalSpiCommandStrobes(cmd); 

}

void spirit_common_test (void)
{
    //SpiritCalibrationGetVcoCalDataRx();
    //SpiritCalibrationGetVcoSelecttion();
    //SpiritCmdStrobeRx(); 
    SpiritCmdStrobeStandby();
}

*/