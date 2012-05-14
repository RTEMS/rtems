/*****************************************************************************/
/*
  High Level Debug Port Functions

 */
/*****************************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include "debugport.h"
#include "m68302scc.h"
#include "bsp.h"

static int initialised;

void debug_port_initialise(void)
{
  scc_initialise(CONSOLE_PORT, CONSOLE_BAUD, FALSE);
#if defined(DEBUG_PORT)
  scc_initialise(DEBUG_PORT, DEBUG_BAUD, FALSE);
#endif
}

unsigned char debug_port_status(const unsigned char status)
{
  if (!initialised)
  {
    initialised = 1;
    debug_port_initialise();
  }

  return scc_status(CONSOLE_PORT, status);
}

unsigned char debug_port_in(void)
{
  if (!initialised)
  {
    initialised = 1;
    debug_port_initialise();
  }

  return scc_in(CONSOLE_PORT);
}

void debug_port_out(const unsigned char character)
{
  if (!initialised)
  {
    initialised = 1;
    debug_port_initialise();
  }

  scc_out(CONSOLE_PORT, character);
}

void debug_port_write(const char *buffer)
{
   while (*buffer != '\0')
   {
     debug_port_out(*buffer++);
   }
}

void debug_port_write_buffer(const char *buffer, unsigned int size)
{
   unsigned int count;
   for (count = 0; count < size; count++)
   {
     debug_port_out(buffer[count]);
   }
}

void debug_port_write_hex_uint(const unsigned int value)
{
   unsigned int bits = sizeof(value) * 8;
   unsigned char c;

   do
   {
     bits -= 4;
     c = (unsigned char) ((value >> bits) & 0x0F);
     if (c < 10)
     {
       c += '0';
     }
     else
     {
       c += 'a' - 10;
     }
     debug_port_out((char) c);
   }
   while (bits);
}

void debug_port_write_hex_ulong(const unsigned long value)
{
   unsigned int bits = sizeof(value) * 8;
   unsigned char c;

   do
   {
     bits -= 4;
     c = (unsigned char) ((value >> bits) & 0x0F);
     if (c < 10)
     {
       c += '0';
     }
     else
     {
       c += 'a' - 10;
     }
     debug_port_out((char) c);
   }
   while (bits);
}

#define BUFFER_SIZE (256)
static char buffer[BUFFER_SIZE];

void debug_port_printf(const char *format, ...)
{
  va_list args;
  int written;

  /*  gain access to the argument list */
  va_start(args, format);

  /* set the trap    */
  buffer[BUFFER_SIZE - 2] = '\xAA';
  buffer[BUFFER_SIZE - 1] = '\x55';

  /* format the string and send to stdout */
  written = vsprintf(buffer, format, args);

  /* try an trap format buffer overflows */
  if ((buffer[BUFFER_SIZE - 2] != '\xAA') ||
      (buffer[BUFFER_SIZE - 1] != '\x55'))
  {
    debug_port_write("debug port buffer overflow, halting...");
    DISABLE_WATCHDOG();
    while (1 == 1);
  }

  /* see if an error occurred, if not flush the output buffer */
  if (written != -1)
  {
    debug_port_write_buffer(buffer, written);
  }
}

void debug_port_banner(void)
{
#define CARD_LABEL "ods68302-" #VARIANT

  debug_port_write("\n\n\r");
  debug_port_write(_Copyright_Notice);
  debug_port_write("\n\r  " CARD_ID "\n\r");
}
