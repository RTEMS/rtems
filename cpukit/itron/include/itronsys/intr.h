/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_INTERRUPT_h_
#define __ITRON_INTERRUPT_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define Interrupt (def_int) Structure
 */

typedef struct t_dint {
  ATR   intatr;   /* interrupt handler attributes */
  FP    inthdr;   /* interrupt handler address */
  /* additional information may be included depending on the implementation */
} T_DINT;

/*
 *  Interrupt Functions
 */

/*
 *  def_int - Define Interrupt Handler
 */

ER def_int(
  UINT dintno,
  T_DINT *pk_dint
);

/*
 *  ret_int - Return from Interrupt Handler
 */

void ret_int( void );

/*
 *  ret_wup - Return and Wakeup Task
 */

void ret_wup(
  ID tskid
);

/*
 *  loc_cpu - Lock CPU
 */

ER loc_cpu( void );

/*
 *  unl_cpu - Unlock CPU
 */

ER unl_cpu( void );

/*
 *  dis_int - Disable Interrupt
 */

ER dis_int(
  UINT eintno
);

/*
 *  ena_int - Enable Interrupt 
 */

ER ena_int(
  UINT eintno
);

/*
 *  chg_iXX - Change Interrupt Mask(Level or Priority)
 */

ER chg_iXX(
  UINT iXXXX
);

/*
 *  ref_iXX - Reference Interrupt Mask(Level or Priority)
 */

ER ref_iXX(
  UINT *p_iXXXX
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

