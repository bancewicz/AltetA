#include "terminal.h"
#include <stdint.h>
#include <string.h>
#include "flags.h"
#include "commands.h"

//------------------------------------------------GLOBALS-------------------------------------------------------------------------
char         rxbuf[64];
uint16_t     rxidx=0;
char*        cmdargs[10];
int          argsidx=0;

extern unsigned char CMDPROCFLAG;

int cmdlistidx=0;

cmdptr commands[CMDLISTVAL];

//--------------------------------------------------------------------------------------------------------------------------------

void add_cmd_list(char* command, void (*function)(void))
{

 if (cmdlistidx>=CMDLISTVAL)
 {
    printf ("\r\n-ERR command list overflow!");
    return;
 }

 strcpy(commands[cmdlistidx].command,command);
 commands[cmdlistidx].function=function;   
                     
 cmdlistidx++;
 
}

void init_cmd_list(void)
{

    int i=0;

    for (i=0; i<CMDLISTVAL; i++)
    {
       strcpy(commands[i].command,"               ");
       commands[i].function=NULL;
    }
    
    cmdlistidx=0;
    
    add_cmd_list("help",help);
	add_cmd_list("reg?",spirit_reg_read);          
    add_cmd_list("reg",spirit_reg_write);
    add_cmd_list("cmd",spirit_cmd_write);
	add_cmd_list("stat?",spirit_status_view);   
	add_cmd_list("standby",spirit_command_standby);   
	add_cmd_list("ready",spirit_command_ready);   
	add_cmd_list("locktx",spirit_command_lock_tx);   
	add_cmd_list("lockrx",spirit_command_lock_rx);   
	add_cmd_list("sleep",spirit_command_sleep);   	
	add_cmd_list("disable",spirit_command_disable);   		   
	add_cmd_list("enable",spirit_command_enable);   	
	add_cmd_list("reset",spirit_command_reset);   	
	add_cmd_list("conf",SpiritBaseConfiguration);   		
	add_cmd_list("cal",SpiritVcoCalibration);   		
	
	
	/*
    add_cmd_list("spinit",spirit_init);                          
    add_cmd_list("spwrite",spirit_write_reg);   
    add_cmd_list("spread",spirit_read_reg); 
    add_cmd_list("spreadall",spirit_read_reg_all); 
    add_cmd_list("spcmd",spirit_write_cmd);
    
//---------------------------------------------------------------------------------------------    
    add_cmd_list("spshdown",spirit_shutdown);
    add_cmd_list("spreset",spirit_reset);         
//---------------------------------------------------------------------------------------------    
    
    
    
    add_cmd_list("reg",spirit_reg_write);  
    add_cmd_list("cmd",spirit_cmd_write);  
    add_cmd_list("test",spirit_common_test);
    */

    
}

void terminal_init (void)
{
    
  memset(cmdargs,0,sizeof(char*)*10);
  argsidx=0;

  memset(rxbuf,0,64);  
  rxidx=0;
  RXFLAGCLR; 
  init_cmd_list();
  printf("\r\n>"); 
     
}

void terminal_proc (char smb)
{
    
   rxbuf[rxidx]=smb;
   rxidx++; 
   if (smb!=13)  putchar(smb);
   if (smb==13) 
   {
		RXFLAGSET;
   }
   
}

void terminal_proc_cmd (void)
{
   

    int i=0;
    char* mnemo=NULL;    
    char* arg=NULL;
    
    rxbuf[rxidx-1]=0;
    //if (rxidx>1) printf ("\r\n-terminalproc command = %s", rxbuf);
    //terminal_init();  
                    
    mnemo =strtok(rxbuf," ");
    if (mnemo==NULL) mnemo =strtok(rxbuf,NULL);  
      
    //printf("\r\n mnemo=%s",mnemo);
             
    arg=strtok(&rxbuf[strlen(mnemo)+1],",");
    while(arg)
    {
        //printf("\r\n arg=%s",arg);
        cmdargs[argsidx]=arg;
        argsidx++;      
        arg=strtok(NULL,","); 
        if (argsidx>9) break;
    }
    
    
    if ((mnemo==NULL)&&(rxidx>1)) printf ("\r\n-ERR bad command!");
    else
    {
        if (rxidx>1)
        {
            for (i=0; i<CMDLISTVAL; i++)
            {
                //if (strstr(mnemo,commands[i].command))   
                if (strcmp(mnemo,commands[i].command)==0)
                {
                     (commands[i].function)();  
                     break;
                }
            } 
        } 
    } 
                      
    if (i==CMDLISTVAL)
    {
        if (mnemo!=NULL) printf ("\r\n-ERR unknown comand [%s]!",rxbuf);    
    } 
		
    terminal_init ();

}


