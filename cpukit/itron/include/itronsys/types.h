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

#ifndef __ITRON_TYPES_h_
#define __ITRON_TYPES_h_

#ifdef __cplusplus
extern "C" {
#endif

/* XXX some of these may need to be moved */
/* 
 *  XXX: These names are too short and conflict with numerous 
 *       off the shell programs including paranoia.
 */

typedef signed8      B;       /* signed 8-bit integer */
typedef signed16     H;       /* signed 16-bit integer */
typedef signed32     W;       /* signed 32-bit integer */
typedef unsigned8    UB;      /* unsigned 8-bit integer */
typedef unsigned16   UH;      /* unsigned 16-bit integer */
typedef unsigned32   UW;      /* unsigned 32-bit integer */

typedef unsigned32   VW;      /* unpredictable data type (32-bit size) */
typedef unsigned16   VH;      /* unpredictable data type (16-bit size) */
typedef unsigned8    VB;      /* unpredictable data type (8-bit size) */

typedef void        *VP;      /* pointer to an unpredictable data type */

typedef void (*FP)();         /* program start address */

/*
 *  6.4 Data Types
 *
 *  The difference between VB, VH and VW and B, H and W is that only the
 *  number of bits is known for the former, not the data type of the
 *  contents.  The latter clearly represent integers.
 */

/*
 *  Data Types Dependent on ITRON Specification 
 *
 *  In order to clarify the meanings of certain parameters, the
 *  following names are used for data types which appear frequently 
 *  and/or have special meanings.
 */

typedef signed32   INT;    /* Signed integer (bit width of processor) */
typedef unsigned32 UINT;   /* Unsigned integer (bit width of processor) */
typedef boolean    BOOL;   /* Boolean value.  TRUE (1) or FALSE (0). */
typedef signed16   FN;     /* Function code. Signed integer. Maximum 2 bytes. */
typedef int        ID;     /* Object ID number (???id) */
                           /*   Value range depends on the system.  Usually */
                           /*   a signed integer.  Certain ID values may */
                           /*   represent objects on other nodes when the */
                           /*   connection function is supported. */

typedef unsigned32 BOOL_ID;/* Boolean value or ID number */

typedef signed32 HNO;      /* Handler number */
typedef signed32 RNO;      /* Rendezvous number */
typedef signed32 NODE;     /* Node Number.  Usually a signed integer. */
typedef signed32 ATR;      /* Object or handler attribute. */
                           /*   An unsigned integer. */
typedef signed32 ER;       /* Error code.  A signed integer. */
typedef unsigned32 PRI;    /* Task priority.  A signed integer. */
typedef signed32 TMO;      /* Timeout value.  A signed integer. */
                           /*   TMO_POL = 0 indicates polling, */
                           /*   while TMO_FEVR = -1 indicates wait forever. */

/*
 *   6.6 Common Constants and Data Structure Packet Formats
 */

#define NADR   (-1)   /* invalid address or pointer value */

#ifndef TRUE
#define TRUE     1    /* true */
#endif

#ifndef FALSE
#define FALSE    0    /* false */
#endif

/*
 *  ATR tskatr, intatr, cycatr, almatr, svcatr, excatr:
 *
 *  TA_ASM indicates that the task or handler is directly started
 *         at the assembly language level.  The TA_ASM attribute has the
 *         opposite meaning of TA_HLNG.
 *
 *  NOTE: Really in <itronsys/task.h>
 */

#if 0
#define TA_ASM    0x00   /* program written in assembly language */
#define TA_HLNG   0x01   /* program written in high-level language */
#endif

/*
 *  TMO tmout:
 */

#define TMO_POL      0    /* polling */
#define TMO_FEVR   (-1)   /* wait forever */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

