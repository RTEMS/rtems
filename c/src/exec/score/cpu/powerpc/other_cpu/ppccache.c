/*
 *  PowerPC Cache enable routines
 *
 *  $Id$
 */

#include <rtems/system.h>

#define PPC_Get_HID0( _value ) \
  do { \
      _value = 0;        /* to avoid warnings */ \
      asm volatile( \
          "mfspr %0, 0x3f0;"     /* get HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)

#define PPC_Set_HID0( _value ) \
  do { \
      asm volatile( \
          "isync;" \
          "mtspr 0x3f0, %0;"     /* load HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)


void powerpc_instruction_cache_enable ()
{
  unsigned32 value;

  /*
   * Enable the instruction cache
   */

  PPC_Get_HID0( value );

  value |= 0x00008000;       /* Set ICE bit */

  PPC_Set_HID0( value );
}

void powerpc_data_cache_enable ()
{
  unsigned32 value;

  /*
   * enable data cache
   */

  PPC_Get_HID0( value );

  value |= 0x00004000;        /* set DCE bit */

  PPC_Set_HID0( value );
}

