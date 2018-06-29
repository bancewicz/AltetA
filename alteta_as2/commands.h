
void help (void);
void spirit_init (void);
void spirit_write_reg (void);
void spirit_read_reg (void);
void spirit_read_reg_all (void);
void spirit_write_cmd (void);
void spirit_shutdown (void);
void spirit_reset (void);
void status_view(void);
void spirit_status_view(void);
void spirit_reg_read(void);
void spirit_reg_write(void);
void spirit_cmd_write(void);

void spirit_command_standby(void);
void spirit_command_ready(void);
void spirit_command_lock_tx(void);
void spirit_command_lock_rx(void);
void spirit_command_sleep(void);
void spirit_command_disable(void);
void spirit_command_enable(void);
void spirit_command_reset(void);
void SpiritBaseConfiguration(void);
void SpiritVcoCalibration(void);


void spirit_common_test (void);



#define ERRORARGUMENT {printf("\r\n-ERR bad command argument!");return;}

extern void sleep (int par);