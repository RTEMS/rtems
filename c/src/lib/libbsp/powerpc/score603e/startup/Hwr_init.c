/*  Hwr_init.c
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

#define PPC603e_SPR_HID0        1008
#define PPC603e_SPR_HID1        1009
#define PPC603e_SPR_IBAT0U       528
#define PPC603e_SPR_IBAT0L       529
#define PPC603e_SPR_DBAT0U       536
#define PPC603e_SPR_DBAT0L       537
#define PPC603e_SPR_IBAT1U       530
#define PPC603e_SPR_IBAT1L       531
#define PPC603e_SPR_DBAT1U       538
#define PPC603e_SPR_DBAT1L       539
#define PPC603e_SPR_IBAT2U       532
#define PPC603e_SPR_IBAT2L       533
#define PPC603e_SPR_DBAT2U       540
#define PPC603e_SPR_DBAT2L       541
#define PPC603e_SPR_IBAT3U       534
#define PPC603e_SPR_IBAT3L       535
#define PPC603e_SPR_DBAT3U       542
#define PPC603e_SPR_DBAT3L       543
#define PPC603e_SPR_DMISS        976
#define PPC603e_SPR_DCMP         977
#define PPC603e_SPR_HASH1        978
#define PPC603e_SPR_HASH2        979
#define PPC603e_SPR_IMISS        980
#define PPC603e_SPR_ICMP         981
#define PPC603e_SPR_RPA          982
#define PPC603e_SPR_SDR1          25
#define PPC603e_SPR_PVR          287
#define PPC603e_SPR_DAR           19
#define PPC603e_SPR_SPRG0        272
#define PPC603e_SPR_SPRG1        273
#define PPC603e_SPR_SPRG2        274
#define PPC603e_SPR_SPRG3        275
#define PPC603e_SPR_DSISR         18
#define PPC603e_SPR_SRR0          26
#define PPC603e_SPR_SRR1          27
#define PPC603e_SPR_TBL_WRITE    284
#define PPC603e_SPR_TBU_WRITE    285
#define PPC603e_SPR_DEC           22
#define PPC603e_SPR_IABR        1010
#define PPC603e_SPR_EAR          282

#define PCI_MEM_CMD   (SCORE603E_PCI_MEM_BASE >> 16)

typedef struct {
  uint32_t          counter_1_100;
  uint32_t          counter_hours;
  uint32_t          counter_min;
  uint32_t          counter_sec;
  uint32_t          counter_month;
  uint32_t          counter_date;
  uint32_t          counter_year;
  uint32_t          counter_day_of_week;

  uint32_t          RAM_1_100;
  uint32_t          RAM_hours;
  uint32_t          RAM_month;
  uint32_t          RAM_date;
  uint32_t          RAM_year;
  uint32_t          RAM_day_of_week;

  uint32_t          interupt_status_mask;
  uint32_t          command_register;
}Harris_RTC;

void init_RTC(void)
{
  volatile Harris_RTC *the_RTC;

  the_RTC = (volatile Harris_RTC *)BSP_RTC_ADDRESS;

  the_RTC->command_register = 0x0;
}

void init_PCI(void)
{
  /* DINK Monitor setsup and uses all 4 BAT registers.  */
  /* The fourth BAT register can be modified to access this area */
}

#define PPC_Get_HID0( _value ) \
  do { \
      _value = 0;        /* to avoid warnings */ \
      __asm__ volatile( \
          "mfspr %0, 0x3f0;"     /* get HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)

#define PPC_Set_HID0( _value ) \
  do { \
      __asm__ volatile( \
          "isync;" \
          "mtspr 0x3f0, %0;"     /* load HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)

void instruction_cache_enable (void)
{
  uint32_t         value;

  /*
   * Enable the instruction cache
   */

  PPC_Get_HID0( value );

  value |= 0x00008000;       /* Set ICE bit */

  PPC_Set_HID0( value );
}

void data_cache_enable (void)
{
  uint32_t         value;

  /*
   * enable data cache
   */

  PPC_Get_HID0( value );

  value |= 0x00004000;        /* set DCE bit */

  PPC_Set_HID0( value );
}
