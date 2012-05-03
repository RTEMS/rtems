/*
 * raw_execption.h
 *
 *	    This file contains implementation of C function to
 *          Instantiate mpc5xx primary exception entries.
 *	    More detailled information can be found on the Motorola
 *	    site and more precisely in the following book:
 *
 *		MPC555/MPC556 User's Manual
 *		Motorola REF : MPC555UM/D Rev. 3, 2000 October 15
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libcpu/powerpc/mpc8xx/exceptions/raw_exception.h:
 *
 *  Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_RAW_EXCEPTION_H
#define _LIBCPU_RAW_EXCEPTION_H

#include <libcpu/vectors.h>

/*
 * Exception Vectors as defined in the MPC555 User's Manual
 */

#define	ASM_RESET_VECTOR 	0x01
#define	ASM_MACH_VECTOR		0x02

#define	ASM_EXT_VECTOR 		0x05
#define	ASM_ALIGN_VECTOR 	0x06
#define	ASM_PROG_VECTOR		0x07
#define	ASM_FLOAT_VECTOR	0x08
#define	ASM_DEC_VECTOR		0x09

#define	ASM_SYS_VECTOR		0x0C
#define	ASM_TRACE_VECTOR	0x0D
#define	ASM_FLOATASSIST_VECTOR	0x0E

#define	ASM_SOFTEMUL_VECTOR	0x10

#define	ASM_IPROT_VECTOR	0x13
#define	ASM_DPROT_VECTOR	0x14

#define ASM_DBREAK_VECTOR	0x1C
#define ASM_IBREAK_VECTOR	0x1D
#define ASM_MEBREAK_VECTOR	0x1E
#define ASM_NMEBREAK_VECTOR	0x1F

#define LAST_VALID_EXC		ASM_NMEBREAK_VECTOR

#ifndef ASM

/*
 * Type definition for raw exceptions.
 */

typedef unsigned char  rtems_vector;
struct 	__rtems_raw_except_connect_data__;
typedef unsigned char 	rtems_raw_except_hdl_size;

typedef struct {
  rtems_vector			vector;
  rtems_exception_handler_t*	raw_hdl;
}rtems_raw_except_hdl;

typedef void (*rtems_raw_except_enable)		(const struct __rtems_raw_except_connect_data__*);
typedef void (*rtems_raw_except_disable)	(const struct __rtems_raw_except_connect_data__*);
typedef int  (*rtems_raw_except_is_enabled)	(const struct __rtems_raw_except_connect_data__*);

typedef struct __rtems_raw_except_connect_data__{
 /*
  * Exception vector (As defined in the manual)
  */
  rtems_vector			exceptIndex;
  /*
   * Exception raw handler. See comment on handler properties below in function prototype.
   */
  rtems_raw_except_hdl	   	hdl;
  /*
   * function for enabling raw exceptions. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage exceptions and thus the
   * "on" routine must enable the except at processor level only.
   *
   */
    rtems_raw_except_enable	on;
  /*
   * function for disabling raw exceptions. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage exceptions and thus the
   * "on" routine must disable the except both at device and PIC level.
   *
   */
  rtems_raw_except_disable	off;
  /*
   * function enabling to know what exception may currently occur
   */
  rtems_raw_except_is_enabled	isOn;
}rtems_raw_except_connect_data;

typedef struct {
  /*
   * size of all the table fields (*Tbl) described below.
   */
  unsigned int	 			exceptSize;
  /*
   * Default handler used when disconnecting exceptions.
   */
  rtems_raw_except_connect_data		defaultRawEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_raw_except_connect_data*	rawExceptHdlTbl;
}rtems_raw_except_global_settings;

/*
 * C callable function enabling to set up one raw idt entry
 */
extern int mpc5xx_set_exception (const rtems_raw_except_connect_data*);

/*
 * C callable function enabling to get one current raw idt entry
 */
extern int mpc5xx_get_current_exception (rtems_raw_except_connect_data*);

/*
 * C callable function enabling to remove one current raw idt entry
 */
extern int mpc5xx_delete_exception (const rtems_raw_except_connect_data*);

/*
 * C callable function enabling to check if vector is valid
 */
extern int mpc5xx_vector_is_valid(rtems_vector vector);

inline static  void* mpc5xx_get_vector_addr(rtems_vector vector)
{
  return ((void*)  (((unsigned) vector) << 8));
}
/*
 * Exception global init.
 */
extern int mpc5xx_init_exceptions (rtems_raw_except_global_settings* config);
extern int mpc5xx_get_exception_config (rtems_raw_except_global_settings** config);

# endif /* ASM */

#define SIZEOF_

#endif
