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

#ifndef __ITRON_STATUS_h_
#define __ITRON_STATUS_h_

#ifdef __cplusplus
extern "C" {
#endif

#define E_OK       0        /* Normal completion */
#define E_SYS      (-5)     /* System error */
#define E_NOMEM    (-10)    /* Insufficient memory */
#define E_NOSPT    (-17)    /* Feature not supported */
#define E_INOSPT   (-18)    /* Feature not supported by ITRON/FILE */
                            /*   specification */
#define E_RSFN     (-20)    /* Reserved function code number */
#define E_RSATR    (-24)    /* Reserved attribute */
#define E_PAR      (-33)    /* Parameter error */
#define E_ID       (-35)    /* Invalid ID number */
#define E_NOEXS    (-52)    /* Object does not exist */
#define E_OBJ      (-63)    /* Invalid object state */
#define E_MACV     (-65)    /* Memory access disabled or memory access */
                            /*   violation */
#define E_OACV     (-66)    /* Object access violation */
#define E_CTX      (-69)    /* Context error */
#define E_QOVR     (-73)    /* Queuing or nesting overflow */
#define E_DLT      (-81)    /* Object being waited for was deleted */
#define E_TMOUT    (-85)    /* Polling failure or timeout exceeded */
#define E_RLWAI    (-86)    /* WAIT state was forcibly released */
#define EN_NOND    (-113)   /* Target node does not exist or cannot be */
                            /*   accessed */
#define EN_OBJNO   (-114)   /* Specifies an object number which could not be */
                            /*   accessed on the target node */
#define EN_PROTO   (-115)   /* Protocol not supported on target node */
#define EN_RSFN    (-116)   /* System call or function not supported on */
                            /*   target node */
#define EN_COMM    (-117)   /* No response from target node */
#define EN_RLWAI   (-118)   /* Connection function response wait state was */
                            /*   forcibly released */
#define EN_PAR     (-119)   /* A value outside the range supported by the */
                            /*   target node and/or transmission packet */
                            /*   format was specified as a parameter */
#define EN_RPAR    (-120)   /* A value outside the range supported by the */
                            /*   issuing node and/or transmission packet */
                            /*   format was returned as a return parameter */
#define EN_CTXID   (-121)   /* An object on another node was specified to */
                            /*   a system call issued from a task in dispatch */
                            /*   disabled state or from a task-independent */
                            /*   portion */
#define EN_EXEC    (-122)   /* System call could not be executed due to */
                            /*   insufficient resources on the target node */
#define EN_NOSPT   (-123)   /* Connection function not supported */


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

