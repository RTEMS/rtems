/*  c4x.h
 *
 *  This file is an example (i.e. "no CPU") of the file which is
 *  created for each CPU family port of RTEMS.
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef _INCLUDE_C4X_h
#define _INCLUDE_C4X_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "no cpu"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(_C30)
#define CPU_MODEL_NAME  "C30"

#elif defined(_C31)
#define CPU_MODEL_NAME  "C31"
 
#elif defined(_C32)
#define CPU_MODEL_NAME  "C32"
 
#elif defined(_C33)
#define CPU_MODEL_NAME  "C33"
 
#elif defined(_C40)
#define CPU_MODEL_NAME  "C40"
 
#elif defined(_C44)
#define CPU_MODEL_NAME  "C44"
 
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "Texas Instruments C3x/C4x"

/*
 *  This port is a little unusual in that even though there are "floating
 *  point registers", the notion of floating point is very inherent to
 *  applications.  In addition, the calling conventions require that
 *  only a few extended registers be preserved across subroutine calls.
 *  The overhead of including these few registers in the basic 
 *  context is small compared to the overhead of managing the notion
 *  of separate floating point contexts.  So we decided to pretend that
 *  there is no FPU on the C3x or C4x.
 */

#define C4X_HAS_FPU  0

/*
 *  Routines to manipulate the bits in the Status Word (ST).
 */

#define C4X_ST_C      0x0001
#define C4X_ST_V      0x0002
#define C4X_ST_Z      0x0004
#define C4X_ST_N      0x0008
#define C4X_ST_UF     0x0010
#define C4X_ST_LV     0x0020
#define C4X_ST_LUF    0x0040
#define C4X_ST_OVM    0x0080
#define C4X_ST_RM     0x0100
#define C4X_ST_CF     0x0400
#define C4X_ST_CE     0x0800
#define C4X_ST_CC     0x1000
#define C4X_ST_GIE    0x2000

#ifndef _TMS320C40
#define C3X_IE_INTERRUPT_MASK_BITS     0xffff
#define C3x_IE_INTERRUPTS_ALL_ENABLED  0x0000
#define C3x_IE_INTERRUPTS_ALL_DISABLED 0xffff
#endif

#ifndef ASM

/*
 *  A nop macro.
 */

#define c4x_nop() \
  __asm__("nop");

/*
 *  Routines to set and clear individual bits in the ST (status word).
 *
 *  cpu_st_bit_clear  - clear bit in ST
 *  cpu_st_bit_set    - set bit in ST
 *  cpu_st_get        - obtain entire ST
 */

#ifdef _TMS320C40
#define c4x_gie_nop()
#else
#define c4x_gie_nop() { c4x_nop(); c4x_nop(); }
#endif

#define cpu_st_bit_clear(_st_bit) \
  do { \
    __asm__("andn %0,st" : : "g" (_st_bit) : "cc"); \
    c4x_gie_nop(); \
  } while (0)

#define cpu_st_bit_set(_st_bit) \
  do { \
    __asm__("or %0,st" : : "g" (_st_bit) : "cc"); \
    c4x_gie_nop(); \
  } while (0)

static inline unsigned int cpu_st_get(void)
{
  register unsigned int st_value;
  __asm__("ldi st, %0" : "=r" (st_value));
  return st_value;
}

/*
 *  Routines to manipulate the Global Interrupt Enable (GIE) bit in
 *  the Status Word (ST).
 *
 *  c4x_global_interrupts_get      - returns current GIE setting
 *  c4x_global_interrupts_disable  - disables global interrupts
 *  c4x_global_interrupts_enable   - enables global interrupts
 *  c4x_global_interrupts_restore  - restores GIE to pre-disable state
 *  c4x_global_interrupts_flash    - temporarily enable global interrupts
 */

#define c4x_global_interrupts_get() \
  (cpu_st_get() & C4X_ST_GIE)
  
#define c4x_global_interrupts_disable() \
  cpu_st_bit_clear(C4X_ST_GIE)

#define c4x_global_interrupts_enable() \
  cpu_st_bit_set(C4X_ST_GIE)

#define c4x_global_interrupts_restore(_old_level) \
  cpu_st_bit_set(_old_level)

#define c4x_global_interrupts_flash(_old_level) \
  do { \
    cpu_st_bit_set(_old_level); \
    cpu_st_bit_clear(C4X_ST_GIE); \
  } while (0)

#ifndef _TMS320C40

/*
 *  Routines to set and get the IF register
 *
 *  c3x_get_if     - obtains IF register
 *  c3x_set_if     - sets IF register
 */

static inline unsigned int c3x_get_if(void)
{
  register unsigned int _if_value;

  __asm__( "ldi if, %0" : "=r" (_if_value) );
  return _if_value;
}

static inline void c3x_set_if(unsigned int _if_value)
{
  __asm__( "ldi %0, if" : : "g" (_if_value) : "if", "cc"); 
}

/*
 *  Routines to set and get the IE register
 *
 *  c3x_get_ie     - obtains IE register
 *  c3x_set_ie     - sets IE register
 */

static inline unsigned int c3x_get_ie(void)
{
  register unsigned int _ie_value;

  __asm__ volatile ( "ldi ie, %0" : "=r" (_ie_value) );
  return _ie_value;
}

static inline void c3x_set_ie(unsigned int _ie_value)
{
  __asm__ volatile ( "ldi %0, ie" : : "g" (_ie_value) : "ie", "cc"); 
}

/*
 *  Routines to manipulates the mask portion of the IE register.
 *
 *  c3x_ie_mask_all     - returns previous IE mask
 *  c3x_ie_mask_restore - restores previous IE mask
 *  c3x_ie_mask_flash   - temporarily restores previous IE mask
 *  c3x_ie_mask_set     - sets a specific set of the IE mask
 */
 
#define c3x_ie_mask_all( _isr_cookie ) \
  do { \
    __asm__("ldi  ie,%0\n" \
            "\tandn 0ffffh, ie" \
          : "=r" (_isr_cookie): : "ie", "cc" ); \
  } while (0)

#define c3x_ie_mask_restore( _isr_cookie )  \
  do { \
    __asm__("or %0, ie" \
          : : "g" (_isr_cookie) : "ie", "cc" ); \
  } while (0)

#define c3x_ie_mask_flash( _isr_cookie ) \
  do { \
    __asm__("or %0, ie\n" \
           "\tandn 0ffffh, ie" \
          : : "g" (_isr_cookie) : "ie", "cc" ); \
  } while (0)

#define c3x_ie_mask_set( _new_mask ) \
  do { unsigned int _ie_mask; \
    unsigned int _ie_value; \
    \
    if ( _new_mask == 0 ) _ie_mask = 0; \
    else                  _ie_mask = 0xffff; \
    _ie_value = c3x_get_ie(); \
    _ie_value &= C4X_IE_INTERRUPT_MASK_BITS; \
    _ie_value |= _ie_mask; \
    c3x_set_ie(_ie_value); \
  } while (0)
#endif
/* end of C3x specific interrupt flag routines */

/*
 *  This is a section of C4x specific interrupt flag management routines.
 */

#ifdef _TMS320C40

/*
 *  Routines to set and get the IIF register
 *
 *  c4x_get_iif     - obtains IIF register
 *  c4x_set_iif     - sets IIF register
 */

static inline unsigned int c4x_get_iif(void)
{
  register unsigned int _iif_value;

  __asm__( "ldi iif, %0" : "=r" (_iif_value) );
  return _iif_value;
}

static inline void c4x_set_iif(unsigned int _iif_value)
{
  __asm__( "ldi %0, iif" : : "g" (_iif_value) : "iif", "cc"); 
}

/*
 *  Routines to set and get the IIE register
 *
 *  c4x_get_iie     - obtains IIE register
 *  c4x_set_iie     - sets IIE register
 */

static inline unsigned int c4x_get_iie(void)
{
  register unsigned int _iie_value;

  __asm__( "ldi iie, %0" : "=r" (_iie_value) );
  return _iie_value;
}

static inline void c4x_set_iie(unsigned int _iie_value)
{
  __asm__( "ldi %0, iie" : : "g" (_iie_value) : "iie", "cc"); 
}

/*
 *  Routines to manipulates the mask portion of the IIE register.
 *
 *  c4x_ie_mask_all     - returns previous IIE mask
 *  c4x_ie_mask_restore - restores previous IIE mask
 *  c4x_ie_mask_flash   - temporarily restores previous IIE mask
 *  c4x_ie_mask_set     - sets a specific set of the IIE mask
 */

#if 0
#warning "C4x IIE masking routines not implemented."
#define c4x_iie_mask_all( _isr_cookie )
#define c4x_iie_mask_restore( _isr_cookie )
#define c4x_iie_mask_flash( _isr_cookie )
#define c4x_iie_mask_set( _new_mask )
#endif

#endif
/* end of C4x specific interrupt flag routines */

/*
 *  Routines to access the Interrupt Trap Table Pointer
 *
 *  c4x_get_ittp    - get ITTP
 *  c4x_set_ittp    - set ITTP
 */

static inline void * c4x_get_ittp(void)
{ 
  register unsigned int _if_value;
  
  __asm__( "ldi if, %0" : "=r" (_if_value) );
  return (void *)((_if_value & 0xffff0000) >> 8); 
}  

static inline void c4x_set_ittp(void *_ittp_value)
{  
  unsigned int _if_value;
  unsigned int _ittp_field;

#ifdef _TMS320C40
  _if_value = c4x_get_iif();
#else
  _if_value = c3x_get_if();
#endif
  _if_value &= 0xffff;
  _ittp_field = (((unsigned int) _ittp_value) >> 8);
  _if_value |= _ittp_field << 16 ;
#ifdef _TMS320C40
  c4x_set_iif( _if_value );
#else
  c3x_set_if( _if_value );
#endif
}  

#endif /* ifndef ASM */

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_C4X_h */
/* end of include file */
