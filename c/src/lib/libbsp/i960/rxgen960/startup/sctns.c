/*-------------------------------------*/
/* sctns.c                             */
/* Last change : 10.10.94              */
/*-------------------------------------*/
#include "sctns.h"
#include "pmc901_memmap.h"
/*#include "led.h"*/
/*-------------------------------------*/

/* Temporary nullation of WRITE LED */
#define WRITE_LED(x) errval =  x;
void ledcyc()
{
	register int i,t;
	extern unsigned char * led_array;
	register unsigned char * la;
	volatile register int k,m;
	la = led_array;
   i = 1;
loop:
	if (i > 9 )
		i = 1;	
	*(unsigned char *) LED_REG = la[i];
	for(t=1; t < 0x10000; t++)
		k = m + 33;
	i++;
	goto loop;
}

void copyCodeToRom(void) 
{
  register int errval = 0;
  unsigned int * s;
  volatile unsigned int * d;
  unsigned int t, i;
  volatile unsigned char * z;
extern unsigned char * led_array;

  WRITE_LED(0x1);
  d = codeRamStart;
  *d = 0;
  if( *d != 0)
	goto error;
  WRITE_LED(0x2);
  *d = 0xffffffff;
  if( *d != 0xffffffff)
	goto error;
  WRITE_LED(0x3);
  t = 1;
  for( i=0; i < 31; i++){
	*d = t;
	if(*d != t)
		goto error;
	t <<= 1;
  }
  z = (unsigned char *)codeRamStart;
  *z = 0;
  if( *z != 0)
	goto error;
  *z = 0xf;
  if( *z != 0xf)
	goto error;
  WRITE_LED(0x4);
  for (s = codeRomStart, d = codeRamStart; d < codeRamEnd; s ++, d ++)  {
    * d = * s;  
  }
  WRITE_LED(0x5);
  for (s = codeRomStart, d = codeRamStart; d < codeRamEnd; s ++, d ++)  {
    if( * d != * s )
	goto error; 
  }
  WRITE_LED(0x6);
  return;

error:
  while(1){
	for(t=1; t < 0x100000; t++)
		*(unsigned char *) LED_REG = errval;
	for(t=1; t < 0x100000; t++)
		*(unsigned char *) LED_REG = led_array[0];
   }
	
}
void zeroBss(void)
{
  unsigned int * p;

  extern unsigned int bssStart_1[], bssEnd_1[];

  for (p = bssStart; p < bssEnd; p ++)  {
    * p = 0;
  }
  for (p = bssStart_1; p < bssEnd_1; p ++)  {
    * p = 0;
  }
}
/*-------------*/
/* End of file */
/*-------------*/
