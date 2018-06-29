
void terminal_init (void);
void terminal_proc (char smb);
void terminal_proc_cmd (void);

typedef struct {

char  command[16];
void (*function)(void);

} cmdptr;

#define CMDLISTVAL 20

extern cmdptr commands[CMDLISTVAL];
extern char*  cmdargs[10];