/************************************************************************
 *
 *  yamon_api.h
 *
 *  YAMON interface definitions
 *
 * ######################################################################
 *
 * mips_start_of_legal_notice
 *
 * Copyright (c) 2003 MIPS Technologies, Inc. All rights reserved.
 *
 *
 * Unpublished rights (if any) reserved under the copyright laws of the
 * United States of America and other countries.
 *
 * This code is proprietary to MIPS Technologies, Inc. ("MIPS
 * Technologies"). Any copying, reproducing, modifying or use of this code
 * (in whole or in part) that is not expressly permitted in writing by MIPS
 * Technologies or an authorized third party is strictly prohibited. At a
 * minimum, this code is protected under unfair competition and copyright
 * laws. Violations thereof may result in criminal penalties and fines.
 *
 * MIPS Technologies reserves the right to change this code to improve
 * function, design or otherwise. MIPS Technologies does not assume any
 * liability arising out of the application or use of this code, or of any
 * error or omission in such code. Any warranties, whether express,
 * statutory, implied or otherwise, including but not limited to the implied
 * warranties of merchantability or fitness for a particular purpose, are
 * excluded. Except as expressly provided in any written license agreement
 * from MIPS Technologies or an authorized third party, the furnishing of
 * this code does not give recipient any license to any intellectual
 * property rights, including any patent rights, that cover this code.
 *
 * This code shall not be exported or transferred for the purpose of
 * reexporting in violation of any U.S. or non-U.S. regulation, treaty,
 * Executive Order, law, statute, amendment or supplement thereto.
 *
 * This code constitutes one or more of the following: commercial computer
 * software, commercial computer software documentation or other commercial
 * items. If the user of this code, or any related documentation of any
 * kind, including related technical data or manuals, is an agency,
 * department, or other entity of the United States government
 * ("Government"), the use, duplication, reproduction, release,
 * modification, disclosure, or transfer of this code, or any related
 * documentation of any kind, is restricted in accordance with Federal
 * Acquisition Regulation 12.212 for civilian agencies and Defense Federal
 * Acquisition Regulation Supplement 227.7202 for military agencies. The use
 * of this code by the Government is further restricted in accordance with
 * the terms of the license agreement(s) and/or applicable contract terms
 * and conditions covering this code from MIPS Technologies or an authorized
 * third party.
 *
 *
 * mips_end_of_legal_notice
 *
 *
 ************************************************************************/

#ifndef YAMON_API_H
#define YAMON_API_H

/************************************************************************
 *  Include files
 ************************************************************************/


/************************************************************************
 *  Definitions
*************************************************************************/

/* Basic types */

typedef unsigned int        t_yamon_uint32;
typedef unsigned short      t_yamon_uint16;
typedef unsigned char       t_yamon_uint8;
typedef signed int	    t_yamon_int32;
typedef signed short	    t_yamon_int16;
typedef signed char	    t_yamon_int8;

typedef unsigned char       t_yamon_bool;

#define YAMON_FALSE	    0
#define YAMON_TRUE	    (!YAMON_FALSE)

/* YAMON Environment variable */
typedef struct
{
    char *name;
    char *val;
}
t_yamon_env_var;

/* Format of application function */
typedef t_yamon_uint32
(*t_yamon_appl_main)(
    t_yamon_uint32  argc,      /* Number of tokens in argv array	*/
    char	    **argv,    /* Array of tokens (first is "go")	*/
    t_yamon_env_var *env,      /* Array of env. variables		*/
    t_yamon_uint32  memsize ); /* Size of memory (byte count)		*/


/* ID of YAMON configuration object */
typedef t_yamon_uint32 t_yamon_syscon_id;


/*  Number used by the exception registration functions in order to register
 *  a default ISR/ESR.
 */
#define YAMON_DEFAULT_HANDLER		    0xfffffff0

/*  Number used by the exception registration functions in order to register
 *  an EJTAG debug exception ESR.
 */
#define YAMON_DEFAULT_EJTAG_ESR		    0xfffffff1

/* Registered Interrupt Service Routine (ISR) */
typedef void (*t_yamon_isr)(void *data);

/* Registered Exception Service Routine (ESR) */
typedef void (*t_yamon_esr)(void);

/* Entry point called by ESRs wishing to pass control back to YAMON */
typedef void (*t_yamon_retfunc)(void);

/* Handle used for deregistration of ISR/ESR */
typedef void *t_yamon_ref;


/* YAMONE Vector table address */
#define YAMON_FUNCTION_BASE		    0x9fc00500

/* YAMON Vector table offsets */
#define YAMON_FUNC_PRINT_COUNT_OFS	    0x04
#define YAMON_FUNC_EXIT_OFS		    0x20
#define YAMON_FUNC_FLUSH_CACHE_OFS	    0x2C
#define YAMON_FUNC_PRINT_OFS		    0x34
#define YAMON_FUNC_REGISTER_CPU_ISR_OFS	    0x38
#define YAMON_FUNC_DEREGISTER_CPU_ISR_OFS   0x3c
#define YAMON_FUNC_REGISTER_IC_ISR_OFS	    0x40
#define YAMON_FUNC_DEREGISTER_IC_ISR_OFS    0x44
#define YAMON_FUNC_REGISTER_ESR_OFS	    0x48
#define YAMON_FUNC_DEREGISTER_ESR_OFS       0x4c
#define YAMON_FUNC_GETCHAR_OFS		    0x50
#define YAMON_FUNC_SYSCON_READ_OFS	    0x54

/* Macro for accessing YAMON function */
#define YAMON_FUNC(ofs)\
    (*(t_yamon_uint32 *)(YAMON_FUNCTION_BASE + (ofs)))


/************************************************************************
 *  Public variables
 ************************************************************************/

/************************************************************************
 *  Public functions
 ************************************************************************/


/************************************************************************
 *
 *                          t_yamon_exit
 *  Description :
 *  -------------
 *
 *  Exit application and return to YAMON.
 *
 *  Parameters :
 *  ------------
 *
 *  'rc' (OUT) : Return code
 *
 *  Return values :
 *  ---------------
 *
 *  None (never returns)
 *
 ************************************************************************/
typedef void
(*t_yamon_exit)(
    t_yamon_uint32 rc );	/* Return code				*/

#define YAMON_FUNC_EXIT( rc )\
    ((t_yamon_exit)( YAMON_FUNC(YAMON_FUNC_EXIT_OFS) ))\
        ( rc )


/************************************************************************
 *
 *                          t_yamon_print
 *  Description :
 *  -------------
 *
 *  Print null-terminated string to tty0.
 *
 *  Parameters :
 *  ------------
 *
 *  'port' (OUT) : Ignored, always prints to tty0. Not included in macro.
 *  's'    (OUT) : String to print.
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
typedef void
(*t_yamon_print)(
    t_yamon_uint32 port, /* Output port (not used, always tty0)		*/
    const char           *s ); /* String to output				*/

#define YAMON_FUNC_PRINT( s )\
    ((t_yamon_print)( YAMON_FUNC(YAMON_FUNC_PRINT_OFS) ))\
        ( 0, s )


/************************************************************************
 *
 *                          t_yamon_print_count
 *  Description :
 *  -------------
 *
 *  Print specified number of characters to tty0.
 *
 *  Parameters :
 *  ------------
 *
 *  'port'  (OUT) : Ignored, always prints to tty0. Not included in macro.
 *  's'     (OUT) : Array of characters to print.
 *  'count' (OUT) : Number of characters to print.
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
typedef void
(*t_yamon_print_count)(
    t_yamon_uint32 port,	/* Output port (not used, always tty0	*/
    char	   *s,		/* String to output			*/
    t_yamon_uint32 count );	/* Number of characters to output	*/

#define YAMON_FUNC_PRINT_COUNT( s, count )\
    ((t_yamon_print_count)( YAMON_FUNC(YAMON_FUNC_PRINT_COUNT_OFS) ))\
        ( 0, s, count )


/************************************************************************
 *
 *                          t_yamon_getchar
 *  Description :
 *  -------------
 *
 *  Get character from tty0 if character is available. Function
 *  returns immediately if no character is available.
 *
 *  Parameters :
 *  ------------
 *
 *  'port'  (OUT) : Ignored, always uses tty0. Not included in macro.
 *  'ch'    (OUT) : Character read (if available).
 *
 *  Return values :
 *  ---------------
 *
 *  YAMON_TRUE if character was available, else YAMON_FALSE.
 *
 ************************************************************************/
typedef t_yamon_bool
(*t_yamon_getchar)(
    t_yamon_uint32 port,	/* Output port (not used, always tty0	*/
    char	   *ch );	/* Character to output			*/

#define YAMON_FUNC_GETCHAR( ch )\
    ((t_yamon_getchar)( YAMON_FUNC(YAMON_FUNC_GETCHAR_OFS) ))\
        ( 0, ch )


/************************************************************************
 *
 *                          t_yamon_syscon_read
 *  Description :
 *  -------------
 *
 *  Read the value of system configuration object given by 'id'.
 *
 *  See syscon_api.h in YAMON source distribution for further details
 *  on object IDs and error codes.
 *
 *  Parameters :
 *  ------------
 *
 *  'id'    (IN)    : Object id.
 *  'param' (INOUT) : Buffer for object value.
 *  'size'  (IN)    : Size of buffer (must match size of object).
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Returned parameter is valid.
 *  Other values indicate error.
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_syscon_read)(
    t_yamon_syscon_id   id,	      /* Object ID			*/
    void                *param,       /* Buffer for object value	*/
    t_yamon_uint32      size);        /* Buffer size (bytes)		*/

#define YAMON_FUNC_SYSCON_READ( id, param, size )\
    ((t_yamon_syscon_read)( YAMON_FUNC(YAMON_FUNC_SYSCON_READ_OFS) ))\
        ( id, param, size )


/************************************************************************
 *
 *                          t_yamon_flush_cache
 *  Description :
 *  -------------
 *
 *  Flush I-or D-cache
 *
 *  Function performs "writeback and invalidate" operations on D-cache
 *  lines and "invalidate" operations on I-cache lines.
 *
 *  Parameters :
 *  ------------
 *
 *  'type' (IN) : Cache to be flushed.
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
typedef void
(*t_yamon_flush_cache)(
#define YAMON_FLUSH_ICACHE	0
#define YAMON_FLUSH_DCACHE 	1
    t_yamon_uint32 type );	/* I- or D-cache indication		*/

#define YAMON_FUNC_FLUSH_CACHE( type )\
    ((t_yamon_flush_cache)( YAMON_FUNC(YAMON_FUNC_FLUSH_CACHE_OFS) ))\
        ( type )


/************************************************************************
 *
 *                          t_yamon_register_esr
 *  Description :
 *  -------------
 *
 *  Registers an exception handler, also known as an "Exception Service
 *  Routine" (ESR) for the specified exception.
 *
 *  Two special exception IDs are defined :
 *      YAMON_DEFAULT_HANDLER used for a default ESR.
 *      YAMON_DEFAULT_EJTAG_ESR used for EJTAG exceptions.
 *
 *  The default ESR is called if no other ESR is registered
 *  for an exception. If no default ESR is registered, a static
 *  (i.e. not registered) "super default" function is invoked.
 *  This function prints out the registers and halts.
 *
 *  Deregistration of an ESR may be be done by calling this function
 *  with 'esr' set to NULL.
 *  An ESR can also be deregistered using the 'yamon_deregister_esr'
 *  function.
 *
 *  An ESR may be registered even if a previously registered
 *  ESR has not been deregistered. In this case the previously
 *  registered ESR is lost.
 *
 *  The ESR will get called with registers in the state they were
 *  when the exception occurred. This includes all CP0 registers and
 *  CPU registers $0..$31, except for k0,k1 ($26,$27).
 *
 *  In case an ESR does not want to handle the exception, it may
 *  call the return function passed in the 'retfunc' parameter.
 *
 *  Case 1 : 'retfunc' called by ESR registered for the
 *           INTERRUPT exception.
 *
 *  We assume an application has registered this ESR and wants
 *  YAMON to process the (remaining) interrupts.
 *
 *  Case 2 :  'retfunc' called by an ESR registered for a specific
 *	      exception (not INTERRUPT).
 *
 *  Default handling will be done.
 *
 *  Case 3 : 'retfunc' is called by the ESR registered as default ESR.
 *
 *  The exception will be handled as though no ESR is registered
 *  (i.e. the "super default" function is called).
 *
 *  Parameters :
 *  ------------
 *
 *  'exception' (IN)  : Exception code
 *		        or YAMON_DEFAULT_HANDLER for a default ESR
 *		        or YAMON_DEFAULT_EJTAG_ESR for ejtag exceptions.
 *  'esr'       (IN)  : Function pointer for ESR.
 *  'ref'	(OUT) : Handle used for deregistration of ESR.
 *  'retfunc'	(OUT) : Pointer to function pointer for the return
 *			function described above.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Registration went well.
 *  Other values indicate error.
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_register_esr)(
    t_yamon_uint32  exception,	     /* Exception identification	*/
    t_yamon_esr     esr,	     /* ESR to be registered		*/
    t_yamon_ref     *ref,	     /* Handle for deregistration	*/
    t_yamon_retfunc *retfunc );      /* Return function			*/

#define YAMON_FUNC_REGISTER_ESR( exc, esr, ref, retfunc )\
    ((t_yamon_register_esr)( YAMON_FUNC(YAMON_FUNC_REGISTER_ESR_OFS) ))\
        ( exc, esr, ref, retfunc )


/************************************************************************
 *
 *                          t_yamon_deregister_esr
 *  Description :
 *  -------------
 *
 *  Deregisters ESR..
 *
 *  Parameters :
 *  ------------
 *
 *  'ref' (IN) : Handle obtained when calling 'yamon_register_esr'.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Deregistration went well.
 *  Other values indicate error.
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_deregister_esr)(
    t_yamon_ref ref );             /* Handle for deregistration		*/

#define YAMON_FUNC_DEREGISTER_ESR( ref )\
    ((t_yamon_deregister_esr)( YAMON_FUNC(YAMON_FUNC_DEREGISTER_ESR_OFS) ))\
        ( ref )


/************************************************************************
 *
 *                          t_yamon_register_cpu_isr
 *  Description :
 *  -------------
 *
 *  Registers an Interrupt Service Routine (ISR) for the specified
 *  CPU interrupt.
 *  The highest service priority is attached to HW-INT5, which is
 *  connected to the CPU-built-in CP0-timer. SW_INT0 gets the lowest
 *  service priority. During registration, the interrupt mask field
 *  in the CPU CP0-status register is updated to enable interrupts
 *  from the specified interrupt source.
 *
 *  A special ID is defined :
 *      YAMON_DEFAULT_HANDLER used for a default ISR.
 *
 *  The default ISR is called if no other ISR is registered
 *  for a CPU interrupt.
 *
 *  Deregistration of the default ISR may be done by calling
 *  this function with 'isr' set to NULL.
 *  Also, a new default ISR may be registered even if a
 *  previously registered ISR has not been deregistered.
 *  ISRs for specific CPU interrupts must be deregistered using
 *  'yamon_deregister_cpu_isr'.
 *
 *  Parameters :
 *  ------------
 *
 *  'cpu_int' (IN)  : CPU interrupt (0..7)
 *		      or YAMON_DEFAULT_HANDLER for a default ISR.
 *  'isr'     (IN)  : Function pointer for ISR.
 *  'data'    (IN)  : Data pointer (may be NULL). Will be passed to
 *		      ISR when called.
 *  'ref'     (OUT) : Handle used for deregistration of ISR.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Registration went well.
 *  Other values indicate error.
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_register_cpu_isr)(
    t_yamon_uint32 cpu_int,	/* CPU interrupt (0..7)			*/
    t_yamon_isr	   isr,		/* ISR to be registered			*/
    void	   *data,   	/* Data reference to be registered	*/
    t_yamon_ref	   *ref );      /* Handle for deregistration		*/

#define YAMON_FUNC_REGISTER_CPU_ISR( cpu_int, isr, data, ref )\
    ((t_yamon_register_cpu_isr)( YAMON_FUNC(YAMON_FUNC_REGISTER_CPU_ISR_OFS) ))\
        ( cpu_int, isr, data, ref )


/************************************************************************
 *
 *                          t_yamon_deregister_cpu_isr
 *  Description :
 *  -------------
 *
 *  Deregisters ISR for CPU interrupt.
 *
 *  Parameters :
 *  ------------
 *
 *  'ref' (IN) : Handle obtained when calling 'yamon_register_cpu_isr'.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Deregistration went well.
 *  Other values indicate error
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_deregister_cpu_isr)(
    t_yamon_ref ref );	             /* Handle for deregistration	*/

#define YAMON_FUNC_DEREGISTER_CPU_ISR( ref )\
    ((t_yamon_deregister_cpu_isr)( YAMON_FUNC(YAMON_FUNC_DEREGISTER_CPU_ISR_OFS) ))\
        ( ref )


/************************************************************************
 *
 *                          t_yamon_register_ic_isr
 *  Description :
 *  -------------
 *
 *  Registers an Interrupt Service Routine (ISR) for the specified
 *  source in the interrupt controller.
 *
 *  A special ID is defined :
 *      YAMON_DEFAULT_HANDLER used for a default ISR.
 *
 *  The default ISR is called if no other ISR is registered
 *  for an interrupt.
 *
 *  Deregistration of the default ISR may be done by calling
 *  this function with 'isr' set to NULL.
 *  Also, a new default ISR may be registered even if a
 *  previously registered ISR has not been deregistered.
 *  ISRs for specific interrupts must be deregistered using
 *  'yamon_deregister_ic_isr'.
 *
 *  Parameters :
 *  ------------
 *
 *  'ic_line' (IN)  : Interrupt source line in Int. Controller
 *		      or YAMON_DEFAULT_HANDLER for a default ISR.
 *  'isr',    (IN)  : Function pointer for user defined ISR.
 *  'data'    (IN)  : Data pointer (may be NULL). Will be passed to
 *		      ISR when called.
 *  'ref',    (OUT) : Handle used for deregistration of ISR.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Registration went well.
 *  Other values indicate error.
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_register_ic_isr)(
    t_yamon_uint32 ic_line,	    /* Interrupt controller line	*/
    t_yamon_isr	   isr,		    /* ISR to be registered		*/
    void	   *data,	    /* Data reference to be registered  */
    t_yamon_ref	   *ref );	    /* Handle for deregistration	*/

#define YAMON_FUNC_REGISTER_IC_ISR( ic_line, isr, data, ref )\
    ((t_yamon_register_ic_isr)( YAMON_FUNC(YAMON_FUNC_REGISTER_IC_ISR_OFS) ))\
        ( ic_line, isr, data, ref )


/************************************************************************
 *
 *                          t_yamon_deregister_ic_isr
 *  Description :
 *  -------------
 *
 *  Deregisters ISR for source in interrupt controller.
 *
 *  Parameters :
 *  ------------
 *
 *  'ref' (IN) : Handle obtained when calling 'yamon_register_ic_isr'.
 *
 *  Return values :
 *  ---------------
 *
 *  0 : Deregistration went well.
 *  Other values indicate error
 *
 ************************************************************************/
typedef t_yamon_int32
(*t_yamon_deregister_ic_isr)(
    t_yamon_ref ref );	             /* Handle for deregistration	*/

#define YAMON_FUNC_DEREGISTER_IC_ISR( ref )\
    ((t_yamon_deregister_ic_isr)( YAMON_FUNC(YAMON_FUNC_DEREGISTER_IC_ISR_OFS) ))\
        ( ref )


#endif /* #ifndef YAMON_API_H */






