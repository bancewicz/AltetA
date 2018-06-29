#include <stdio.h>
#include <stdint.h>


typedef struct {
uint8_t f0 : 1;
uint8_t f1 : 1;
uint8_t f2 : 1;
uint8_t f3 : 1;
uint8_t f4 : 1;
uint8_t f5 : 1;
uint8_t f6 : 1;
uint8_t f7 : 1;
} struct_flags;

extern struct_flags flags1;
/*
f0 - usart terminal string received


*/

#define RXFLAGCLR   flags1.f0=0
#define RXFLAGSET   flags1.f0=1
#define RXFLAG      flags1.f0==1