/*
 *  Thumb mode does not support multi-level ISR, only disable and enable.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>

#if (defined(__THUMB_INTERWORK__) || defined(__thumb__))

/*
 *
 *  _CPU_ISR_Get_level_Thumb - returns the current interrupt level
 */
uint32_t _CPU_ISR_Get_level_Thumb(void)       __attribute__ ((naked));
uint32_t _CPU_ISR_Disable_Thumb(void )        __attribute__ ((naked));
void     _CPU_ISR_Enable_Thumb(int  _level )  __attribute__ ((naked));
void     _CPU_ISR_Flash_Thumb(int _level )    __attribute__ ((naked));
void     _CPU_ISR_Set_level_Thumb(int  new_level )  __attribute__ ((naked));

/*
 * prevent multipule enable/disable ISR
 * 0: ISR enabled, >1 ISR disabled
 */

/*
 * Set the CPSR bit 6,7 to 1 disables FIQ and IRQ
 * Set the CPSR bit 6,7 to 0 enables FIQ and IRQ
 */

#define str(x) #x
#define xstr(x) str(x)
#define L(x) #x "_" xstr(__LINE__)

/*
 * Switch to ARM mode Veneer,ugly but safe
 */
#define TO_ARM_MODE(x) \
    asm volatile ( \
        ".code  16           \n" \
        L(x) "_thumb:        \n" \
        ".align 2            \n" \
        "push {lr}           \n" \
        "adr %0, "L(x) "_arm \n" \
        "bl " L(x)"          \n" \
        "pop    {pc}         \n" \
        ".balign 4           \n" \
        L(x) ":              \n" \
        "bx     %0           \n" \
        "nop                 \n" \
        ".pool               \n" \
        ".code  32           \n" \
        L(x) "_arm:          \n" \
    : "=&r" (reg))

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 */

uint32_t _CPU_ISR_Disable_Thumb(void )
{
  int reg=0;

  TO_ARM_MODE(disable);
  asm volatile( 
      ".code  32             \n"
      "STMDB sp!, {r1}       \n"
      "MRS    r0, CPSR       \n"
      "DISABLE_ARM:          \n"
      "ORR  r1, r0, #0xc0    \n"
      "MSR  CPSR, r1         \n"
      "pop  {r1}             \n"
      "BX LR                 \n"
      ".code  16             \n"
  );
}

/*
 *  Enable interrupts to the previous level (returned by
 *  _CPU_ISR_Disable_Thumb).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _level is not modified. I do not think _level is useful in this
 */

void _CPU_ISR_Enable_Thumb(int  _level )
{
  int reg=0;

  TO_ARM_MODE(enable);
  asm volatile(
      ".code  32              \n"
      "ENABLE_ARM:            \n"
      "MSR    CPSR, %0        \n"
      /* Return back to thumb.*/
      "BX     R14             \n"
      ".code  16              \n"
      : : "r"(_level)
  );
}

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 */
void _CPU_ISR_Flash_Thumb(int _level )
{
  int reg=0;

  TO_ARM_MODE(flash);
  asm volatile(
      ".code  32              \n"
      "FLASH_ARM:             \n"
      "MRS %0, CPSR           \n"
      "BIC %0, %0, #0xC0      \n"
      /* enable the irq*/ 
      "MSR    CPSR_c, %0      \n"
      "ORR    %0, %0, #0xc0   \n"
      "MSR    CPSR_c, %0      \n"
      "BX     R14             \n"
      ".code  16              \n"
      :"=&r"(reg) : "r" (_level)
  );
}

/*
 *  Map interrupt level in task mode onto the hardware that the CPU
 *  actually provides.  Currently, interrupt levels which do not
 *  map onto the CPU in a generic fashion are undefined.  Someday,
 *  it would be nice if these were "mapped" by the application
 *  via a callout.  For example, m68k has 8 levels 0 - 7, levels
 *  8 - 255 would be available for bsp/application specific meaning.
 *  This could be used to manage a programmable interrupt controller
 *  via the rtems_task_mode directive.
 *
 *  The get routine usually must be implemented as a subroutine.
 *  ARM/Thumb dont distinguishd the interrupt levels
 */

void _CPU_ISR_Set_level_Thumb(int new_level)
{
  int reg = 0; /* to avoid warning */
  TO_ARM_MODE(SetISR);
  asm volatile (\
      ".code  32          \n" \
      "SET_LEVEL_ARM:     \n" \
      "MRS  %0, CPSR      \n" \
      "BIC  %0, %0, #0xC0 \n" \
      "MSR  CPSR_c, %0        \n" \
      "BX   lr                        \n" \
      ".code  16                      \n" \
      : "=r" (reg)            \
      : "0" (reg));
 }

uint32_t  _CPU_ISR_Get_level_Thumb( void )
{
    uint32_t   reg = 0; /* to avoid warning */
    TO_ARM_MODE(GetISR); \
    asm volatile (\
                        ".code  32                      \n"     \
                        "GET_ISR_ARM:           \n" \
                        "MRS  r0, cpsr          \n" \
                        "AND  r0, r0, #0xC0 \n" \
                        "EOR    r0, r0, #0xC0 \n" \
                        "BX LR                          \n" \
                        ".code  16                      \n" \
                        ".thumb_func            \n"  );
}

#endif
