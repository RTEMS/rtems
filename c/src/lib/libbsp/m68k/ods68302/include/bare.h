/*****************************************************************************/
/*
  Card Definition for a bare board.

  This is an example file which actually builds a BSP for a 68302 card
  called an MVF (Multi-Voice-Frequency). The card is one of a range
  which run in a 100Mbit voice/video/data switch used for high end
  applications such as Air Traffic Control. The transport is
  FDDI-2. Yes it alive and well and working in real systems.

  Chip selects are programmed as required. Three are controlled in the
  boot code. They are RAM, ROM, and peripherals. You can optionally
  configure the other two chip selects.

  SYSTEM_CLOCK - You must defined this. It is used for setting the
  baud rate.

  CSEL_ROM, CSEL_RAM - Must be defined, and made to be a single number
  with brackets.

  ROM_WAIT_STATES, RAM_WAIT_STATES - Must be defined. This sets the
  speed for the ROM and RAM.

  ROM and RAM size is passed on the command line. The makefile holds
  them. This allows a single place to defined it. The makefile allows
  them to be passed to the linker.

  CSEL_1, CSEL_2 - If defined the other macros needed to define the
  chip select must be defined. If not defined they are not programmed
  and registers are left in the reset state.

  Card Specific Devices - The MVF card uses a chip select to address a
  range of peripherials (CSEL_2). These include front panel leds, and
  4 digit diagnostic display device. Put what ever you need.

  LED_CONTROL - If defined the boot code will set leds as it goes.

  UPDATE_DISPLAY - A four digit display device will also be updated to
  show the boot state.

  CARD_PA, CARD_PB - The default configuration, data direction and
  data must be specified.

  This file allows a range of common parameters which vary from one
  variant of card to another to placed in a central file.

*/
/*****************************************************************************/

#ifndef _BARE_H_
#define _BARE_H_

#if __cplusplus
extern "C"
{
#endif

/* name of the card */
#define CARD_ID "m68302-odsbare"

/* speed of the processor */
#define SYSTEM_CLOCK (15360000)

#define SCR_DEFAULT  (RBIT_SCR_IPA | RBIT_SCR_HWT | RBIT_SCR_WPV | RBIT_SCR_ADC | \
                      RBIT_SCR_HWDEN | RBIT_SCR_HWDCN1 | RBIT_SCR_EMWS)

/* define the chip selects */
#define CSEL_ROM           0            /* token pasted so no brackets */
#define ROM_WAIT_STATES    (OR_DTACK_1) /* 100nsec at 16MHz */
#define CSEL_RAM           3
#define RAM_WAIT_STATES    (OR_DTACK_0) /* 70nsec at 16MHz */

/* The remaining chip selects are called 1 and 2 */
/*
#define CSEL_1             1
#define CSEL_1_BASE        (0x00?00000)
#define CSEL_1_SIZE        (0x00?00000)
#define CSEL_1_WAIT_STATES (OR_DTACK_1)
*/
#define CSEL_2             2
#define CSEL_2_BASE        (0x00800000)
#define CSEL_2_SIZE        (0x00040000)
#define CSEL_2_WAIT_STATES (OR_DTACK_EXT)

/*
 * Need to define a watchdog period
 */
#define WATCHDOG_TIMEOUT_PERIOD (3000 * 2)

/*
 * Console and debug port allocation, 0=SCC1, 2=SCC3
 */

#define CONSOLE_PORT  1
#define CONSOLE_BAUD  SCC_9600
#define DEBUG_PORT    2
#define DEBUG_BAUD    SCC_57600

/* ----
   Parallel Port Configuration, and default data directions

   PORT  BITS - NAME                 , WHO       , DEFAULT WHAT
   ------------------------------------------------------------
   PPA:: 1: 0 - Serial               , PERIPHERAL, -
   PPA:: 7: 2 - MVF_PPA:7:2          , IO        , INPUTS
   PPA:: 9: 8 - Serial               , PERIPHERAL, -
   PPA::15:10 - MVF_PPB:15:10        , IO        , INPUTS

   PPB:: 1: 0 - Setup                , IO        , INPUTS
   PPB:: 3: 2 - SYNC_HIGHWAY_1:2     , IO        , INPUTS
              - SYNC_HIGHWAY_2:3     , IO        , INPUTS
   PPB:: 4: 4 - HARDWARE_RESET:4     , IO        , OUTPUT
   PPB:: 6: 5 - SOFTWARE_OVERRIDE_1:6, IO        , OUTPUT
              - SOFTWARE_OVERRIDE_2:5, IO        , OUTPUT
   PPB:: 7: 7 - Watchdog             , PERIPHERAL, -
   PPB::11: 8 - Interrupt            , PERIPHERAL, -
   PPB::15:12 - Not implemented on the 68302

         15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
   ------------------------------------------------------
   PACNT  0  0  0  0  0  0  1  1  0  0  0  0  0  0  1  1 = 0x0303
   PBCNT  -  -  -  -  -  -  -  -  1  0  0  0  0  0  0  0 = 0x0080

   PADDR  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 = 0x0000
   PBDDR  0  0  0  0  0  0  0  0  0  1  1  1  0  0  0  0 = 0x0070

   PADAT  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 = 0x0000

 */
#define CARD_PA_CONFIGURATION       0x0303
#define CARD_PB_CONFIGURATION       0x0080

#define CARD_PA_DEFAULT_DIRECTIONS  0x0000
#define CARD_PB_DEFAULT_DIRECTIONS  0x0070

#define CARD_PA_DEFAULT_DATA        0x0000
#define CARD_PB_DEFAULT_DATA        (HARDWARE_RESET_DISABLE | \
                                     SOFTWARE_OVERRIDE_1_DISABLE | \
                                     SOFTWARE_OVERRIDE_2_DISABLE)

/* these are specific to the card and are not required */
#define HARDWARE_RESET_ENABLE       0x0000
#define HARDWARE_RESET_DISABLE      0x0010

#define SOFTWARE_OVERRIDE_1_ENABLE  0x0000
#define SOFTWARE_OVERRIDE_1_DISABLE 0x0040
#define SOFTWARE_OVERRIDE_2_ENABLE  0x0000
#define SOFTWARE_OVERRIDE_2_DISABLE 0x0020

/*
 * Card Specific Devices, these are not required. Add what ever you
 * like here.
 */

/* Write */
#define WRITE_REGISTER_8(address, data) \
                 *((uint8_t*) (address)) = ((uint8_t) (data))
#define WRITE_REGISTER_16(address, data) \
                 *((uint16_t*) (address)) = ((uint16_t) (data))
#define WRITE_REGISTER_32(address, data) \
                 *((uint32_t*) (address)) = ((uint32_t) (data))
/* Read */
#define READ_REGISTER_8(address, data) data = *((uint8_t*) (address))
#define READ_REGISTER_16(address, data) data = *((uint16_t*) (address))
#define READ_REGISTER_32(address, data) data = *((uint32_t*) (address))

/* CS2 : Peripherials */
#define PERIPHERIALS_BASE         (CSEL_2_BASE)

#define STATUS_REGISTER_BASE      (PERIPHERIALS_BASE + 0x00000000)

#define PERIPHERIALS_SIZE         (0x00040000)

#define LEDS_BASE                 (PERIPHERIALS_BASE + 0x00004000)
#define MSC_BASE                  (PERIPHERIALS_BASE + 0x00008000)
#define SPARE_1_BASE              (PERIPHERIALS_BASE + 0x0000C000)
#define DISPLAY_BASE              (PERIPHERIALS_BASE + 0x00010000)
#define PIO_INT_BASE              (PERIPHERIALS_BASE + 0x00014000)
#define UART_BASE                 (PERIPHERIALS_BASE + 0x00018000)
#define PIA_BASE                  (PERIPHERIALS_BASE + 0x0001C000)

#define LED_1         0x0002
#define LED_1_GREEN   0xFFFD
#define LED_1_RED     0xFFFF
#define LED_1_OFF     0xFFFC

#define LED_2         0x0001
#define LED_2_GREEN   0xFFFE
#define LED_2_RED     0xFFFF
#define LED_2_OFF     0xFFFC

#define LED_3         0x0000
#define LED_3_GREEN   0xFFFC
#define LED_3_RED     0xFFFC
#define LED_3_OFF     0xFFFC

#define LED_4         0x0000
#define LED_4_GREEN   0xFFFC
#define LED_4_RED     0xFFFC
#define LED_4_OFF     0xFFFC

#define LED_5         0x0000
#define LED_5_GREEN   0xFFFC
#define LED_5_RED     0xFFFC
#define LED_5_OFF     0xFFFC

#define LED_6         0x0000
#define LED_6_GREEN   0xFFFC
#define LED_6_RED     0xFFFC
#define LED_6_OFF     0xFFFC

#define LED_7         0x0000
#define LED_7_GREEN   0xFFFC
#define LED_7_RED     0xFFFC
#define LED_7_OFF     0xFFFC

#define LED_8         0x0000
#define LED_8_GREEN   0xFFFC
#define LED_8_RED     0xFFFC
#define LED_8_OFF     0xFFFC

#define MAKE_LED(L1, L2, L3, L4) ((L1 & LED_1) | (L2 & LED_2) | (L3 & LED_3) | (L4 & LED_4))

#define LED_CONTROL(L1, L2, L3, L4, L5, L6, L7, L8) \
            WRITE_REGISTER_16(LEDS_BASE, MAKE_LED(L1, L2, L3, L4))

 /* update the display, needs a long word */
#define UPDATE_DISPLAY(LongWordPtr) \
         ( WRITE_REGISTER_16(DISPLAY_BASE, *(((uint8_t*) LongWordPtr) + 3)), \
           WRITE_REGISTER_16(DISPLAY_BASE + 2, *(((uint8_t*) LongWordPtr) + 2)), \
           WRITE_REGISTER_16(DISPLAY_BASE + 4, *(((uint8_t*) LongWordPtr) + 1)), \
           WRITE_REGISTER_16(DISPLAY_BASE + 6, *((uint8_t*) LongWordPtr)) )

/* make a better test, say switches */
#if defined(GDB_MONITOR_ACTIVE)
#define GDB_RUN_MONITOR() (1 == 1)
#else
#define GDB_RUN_MONITOR() (1 == 0)
#endif

#if __cplusplus
}
#endif
#endif
